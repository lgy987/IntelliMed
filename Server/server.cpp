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
        QString userId = query.value("id").toString();
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
QJsonObject Server::handleTokenLogin(const QJsonObject &request) {
    QString token = request["token"].toString();

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE token = :token AND token_expiry > :now");
    query.bindValue(":token", token);
    query.bindValue(":now", QDateTime::currentSecsSinceEpoch());

    if (query.exec() && query.next()) {
        // Token valid, renew it
        QString userId = query.value("id").toString();
        QString username = query.value("username").toString();
        return createSessionForUser(userId, username);
    } else {
        QJsonObject reply;
        reply["action"] = "login";
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
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM users WHERE username = :username OR email = :email");
        checkQuery.bindValue(":username", username);
        checkQuery.bindValue(":email", email);

        if (checkQuery.exec() && checkQuery.next()) {
            int count = checkQuery.value(0).toInt();
            if (count > 0) {
                reply["status"] = "error";

                // Determine which field is duplicate
                QSqlQuery usernameQuery;
                usernameQuery.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
                usernameQuery.bindValue(":username", username);
                usernameQuery.exec();
                usernameQuery.next();
                if (usernameQuery.value(0).toInt() > 0) {
                    reply["message"] = "用户名已存在";
                } else {
                    reply["message"] = "邮箱已存在";
                }
                return reply;
            }
        }

        // If we reach here, both username and email are unique
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password, email) VALUES (:username, :password, :email)");
        insertQuery.bindValue(":username", username);
        insertQuery.bindValue(":password", password);
        insertQuery.bindValue(":email", email);

        if (insertQuery.exec()) {
            reply["status"] = "ok";
            reply["message"] = "注册成功";
        } else {
            reply["status"] = "error";
            reply["message"] = insertQuery.lastError().text();
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

QJsonObject Server::createSessionForUser(const QString &userId, const QString &username) {
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
    update.bindValue(":id", userId.toInt());
    update.exec();

    if (!update.exec()) {
        qWarning() << "Failed to update token:" << update.lastError().text();
    }

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
