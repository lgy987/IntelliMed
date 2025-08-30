#include "server.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

Server::Server(QObject *parent) : QTcpServer(parent) {
    if (!listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server could not start!";
    } else {
        qDebug() << "Server listening on port 12345";
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    clients << client;

    connect(client, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &Server::onDisconnected);

    qDebug() << "New client connected:" << socketDescriptor;
}

void Server::onReadyRead() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    while (client->canReadLine()) {
        QByteArray line = client->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) continue;

        QJsonObject request = doc.object();
        QJsonObject reply = handleAction(request);

        sendResponse(client, reply);
    }
}

// Handle different actions based on request
QJsonObject Server::handleAction(const QJsonObject &request) {
    QString action = request["action"].toString();
    if (action == "login") {
        return handleLogin(request);
    } else if (action == "tokenLogin") {
        return handleTokenLogin(request);
    } else if (action == "signup") {
        return handleSignUp(request);
    } else if (action == "getPersonalInfo") {
        return handleGetPersonalInfo(request);
    } else if (action == "updatePersonalInfo") {
        return handleUpdatePersonalInfo(request);
    } else {
        QJsonObject reply;
        reply["action"] = "";
        reply["status"] = "error";
        reply["message"] = "未知操作";
        return reply;
    }
}

// Handle normal username/password login
QJsonObject Server::handleLogin(const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        int userId = query.value("id").toInt();
        return createSessionForUser(userId, username);
    } else {
        QJsonObject reply;
        reply["action"] = "login";
        reply["status"] = "error";
        reply["message"] = "登录信息无效";
        return reply;
    }
}

// Handle token-based login
int Server::checkToken(const QString &token) {
    if (token.isEmpty()) return -1;

    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE token = :token AND token_expiry > :now");
    query.bindValue(":token", token);
    query.bindValue(":now", QDateTime::currentSecsSinceEpoch());

    if (query.exec() && query.next()) {
        return query.value("id").toInt(); // valid token -> userId
    }
    return -1; // invalid token
}

// Revised handleTokenLogin using checkToken
QJsonObject Server::handleTokenLogin(const QJsonObject &request) {
    QString token = request["token"].toString();
    int userId = checkToken(token);

    QJsonObject reply;
    reply["action"] = "login";

    if (userId > 0) {
        // Token valid, renew session
        QSqlQuery query;
        query.prepare("SELECT username FROM users WHERE id = :id");
        query.bindValue(":id", userId);
        QString username;
        if (query.exec() && query.next()) {
            username = query.value("username").toString();
        }
        return createSessionForUser(userId, username);
    } else {
        reply["status"] = "retry";
        return reply;
    }
}

// Handle user signup
QJsonObject Server::handleSignUp(const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    QString email    = request["email"].toString();

    QJsonObject reply;
    reply["action"] = "signup";
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        reply["status"] = "error";
        reply["message"] = "所有字段均为必填项";
    } else {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password, email) "
                            "VALUES (:username, :password, :email)");
        insertQuery.bindValue(":username", username);
        insertQuery.bindValue(":password", password);
        insertQuery.bindValue(":email", email);

        if (!insertQuery.exec()) {
            QSqlError err = insertQuery.lastError();
            if (err.nativeErrorCode() == "2067" || err.databaseText().contains("UNIQUE")) {
                // 2067 is SQLite constraint code; databaseText often mentions UNIQUE
                reply["status"] = "error";
                if (err.databaseText().contains("username"))
                    reply["message"] = "用户名已存在";
                else if (err.databaseText().contains("email"))
                    reply["message"] = "邮箱已存在";
                else
                    reply["message"] = "唯一约束冲突";
            } else {
                reply["status"] = "error";
                reply["message"] = err.text();
            }
        } else {
            reply["status"] = "ok";
            reply["message"] = "注册成功";
        }
    }
    return reply;
}

// Send a JSON reply to client
void Server::sendResponse(QTcpSocket *client, const QJsonObject &reply) {
    QJsonDocument responseDoc(reply);
    client->write(responseDoc.toJson(QJsonDocument::Compact) + "\n");
}

void Server::onDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
        qDebug() << "Client disconnected.";
    }
}

QJsonObject Server::createSessionForUser(const int &userId, const QString &username) {
    // Generate token
    QString token = generateToken();

    // Optional: set token expiry (e.g., 1 hour from now)
    qint64 now = QDateTime::currentSecsSinceEpoch();
    qint64 duration = 24 * 3600; // 1 day
    qint64 expiry = now + duration;

    // Update database
    QSqlQuery update;
    update.prepare("UPDATE users SET token = :token, token_expiry = :expiry WHERE id = :id");
    update.bindValue(":token", token);
    update.bindValue(":expiry", static_cast<qint64>(expiry));
    update.bindValue(":id", userId);
    update.exec();

    // Prepare reply
    QJsonObject reply;
    reply["action"] = "login";
    reply["status"] = "ok";
    reply["token"] = token;
    reply["userId"] = userId;
    reply["username"] = username;

    return reply;
}

QString Server::generateToken() {
    QByteArray randomBytes = QUuid::createUuid().toByteArray();
    QString token = QCryptographicHash::hash(randomBytes, QCryptographicHash::Sha256).toHex();
    return token;
}

QJsonObject Server::handleGetPersonalInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "getPersonalInfo";

    QString token = request["token"].toString();
    int userId = checkToken(token);
    if (userId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的token，请重新登录";
        return reply;
    }

    QSqlQuery query;
    // LEFT JOIN to make sure we still get username/email even if no personal_info row exists yet
    query.prepare(R"(
        SELECT u.username, u.email,
               p.name, p.id_number, p.phone_number
        FROM users u
        LEFT JOIN personal_info p ON u.id = p.id
        WHERE u.id = :id
    )");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        reply["status"]       = "ok";
        reply["username"]     = query.value("username").toString();
        reply["email"]        = query.value("email").toString();
        reply["name"]         = query.value("name").toString();
        reply["id_number"]    = query.value("id_number").toString();
        reply["phone_number"] = query.value("phone_number").toString();
    } else {
        // This should never happen if userId is valid, but fallback anyway
        reply["status"]       = "ok";
        reply["username"]     = "";
        reply["email"]        = "";
        reply["name"]         = "";
        reply["id_number"]    = "";
        reply["phone_number"] = "";
    }

    return reply;
}

QJsonObject Server::handleUpdatePersonalInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "updatePersonalInfo";

    QString token = request["token"].toString();
    int userId = checkToken(token);
    if (userId <= 0) {
        reply["status"] = "error";
        reply["message"] = "userId不能为空或无效";
        return reply;
    }

    QString name = request["name"].toString().trimmed();
    QString idNumber = request["id_number"].toString().trimmed();
    QString phone = request["phone_number"].toString().trimmed();

    QSqlQuery query;
    // Use INSERT with ON CONFLICT(id) DO UPDATE for upsert
    query.prepare("INSERT INTO personal_info (id, name, id_number, phone_number) "
                  "VALUES (:id, :name, :id_number, :phone_number) "
                  "ON CONFLICT(id) DO UPDATE SET "
                  "    name = excluded.name, "
                  "    id_number = excluded.id_number, "
                  "    phone_number = excluded.phone_number;");
    query.bindValue(":id", userId);
    query.bindValue(":name", name);
    query.bindValue(":id_number", idNumber);
    query.bindValue(":phone_number", phone);

    if (query.exec()) {
        reply["status"] = "ok";
        reply["message"] = "更新成功";
    } else {
        reply["status"] = "error";
        reply["message"] = "更新失败：" + query.lastError().text();
    }

    return reply;
}
