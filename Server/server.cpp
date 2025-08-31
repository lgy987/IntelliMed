#include "server.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "doctoradviceserver.h"

Server::Server(QObject *parent) : QTcpServer(parent) {
    if (!listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server could not start!";
    } else {
        qDebug() << "Server listening on port 12345";
    }
    //doctorAdviceServer.start();
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
    } else if (action == "doctorLogin") {
        return handleDoctorLogin(request);
    } else if (action == "doctorTokenLogin") {
        return handleDoctorTokenLogin(request);
    } else if (action == "doctorSignup") {
        return handleDoctorSignUp(request);
    } else if (action == "doctorGetPersonalInfo") {
        return handleDoctorGetPersonalInfo(request);
    } else if (action == "doctorUpdatePersonalInfo") {
        return handleDoctorUpdatePersonalInfo(request);
    } else if (action == "getSessionInfo") {
        return handleGetSessionInfo(request);
    } else if (action == "doctorGetSessionInfo") {
        return handleDoctorGetSessionInfo(request);
    } else if (action == "endSession") {
        return handleEndSession(request);
    } else if (action == "getMessages") {
        return handleGetMessages(request);
    } else if (action == "sendMessage") {
        return handleSendMessage(request);
    } else if (action == "doctorAdvice") {
        return forwardDoctorAdviceRequest(request);
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

QJsonObject Server::handleDoctorLogin(const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QSqlQuery query;
    query.prepare("SELECT * FROM doctors WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        int userId = query.value("id").toInt();
        return createSessionForDoctor(userId, username);
    } else {
        QJsonObject reply;
        reply["action"] = "doctorLogin";
        reply["status"] = "error";
        reply["message"] = "登录信息无效";
        return reply;
    }
}

// Handle token-based login
int Server::checkDoctorToken(const QString &token) {
    if (token.isEmpty()) return -1;

    QSqlQuery query;
    query.prepare("SELECT id FROM doctors WHERE token = :token AND token_expiry > :now");
    query.bindValue(":token", token);
    query.bindValue(":now", QDateTime::currentSecsSinceEpoch());

    if (query.exec() && query.next()) {
        return query.value("id").toInt(); // valid token -> userId
    }
    return -1; // invalid token
}

// Revised handleTokenLogin using checkToken
QJsonObject Server::handleDoctorTokenLogin(const QJsonObject &request) {
    QString token = request["token"].toString();
    int userId = checkDoctorToken(token);

    QJsonObject reply;
    reply["action"] = "doctorLogin";

    if (userId > 0) {
        // Token valid, renew session
        QSqlQuery query;
        query.prepare("SELECT username FROM doctors WHERE id = :id");
        query.bindValue(":id", userId);
        QString username;
        if (query.exec() && query.next()) {
            username = query.value("username").toString();
        }
        return createSessionForDoctor(userId, username);
    } else {
        reply["status"] = "retry";
        return reply;
    }
}

// Handle user signup
QJsonObject Server::handleDoctorSignUp(const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    QString email    = request["email"].toString();

    QJsonObject reply;
    reply["action"] = "doctorSignup";
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        reply["status"] = "error";
        reply["message"] = "所有字段均为必填项";
    } else {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO doctors (username, password, email) "
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

QJsonObject Server::createSessionForDoctor(const int &userId, const QString &username) {
    // Generate token
    QString token = generateToken();

    // Optional: set token expiry (e.g., 1 hour from now)
    qint64 now = QDateTime::currentSecsSinceEpoch();
    qint64 duration = 24 * 3600; // 1 day
    qint64 expiry = now + duration;

    // Update database
    QSqlQuery update;
    update.prepare("UPDATE doctors SET token = :token, token_expiry = :expiry WHERE id = :id");
    update.bindValue(":token", token);
    update.bindValue(":expiry", static_cast<qint64>(expiry));
    update.bindValue(":id", userId);
    update.exec();

    // Prepare reply
    QJsonObject reply;
    reply["action"] = "doctorLogin";
    reply["status"] = "ok";
    reply["token"] = token;
    reply["userId"] = userId;
    reply["username"] = username;

    return reply;
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

QJsonObject Server::handleDoctorGetPersonalInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "doctorGetPersonalInfo";

    QString token = request["token"].toString();
    int userId = checkDoctorToken(token);
    if (userId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的token，请重新登录";
        return reply;
    }

    QSqlQuery query;
    // LEFT JOIN to make sure we still get username/email even if no personal_info row exists yet
    query.prepare(R"(
        SELECT u.username, u.email,
               p.name, p.department, p.title, p.description
        FROM doctors u
        LEFT JOIN doctor_personal_info p ON u.id = p.id
        WHERE u.id = :id
    )");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        reply["status"]       = "ok";
        reply["username"]     = query.value("username").toString();
        reply["email"]        = query.value("email").toString();
        reply["name"]         = query.value("name").toString();
        reply["department"]   = query.value("department").toString();
        reply["title"]        = query.value("title").toString();
        reply["description"]  = query.value("description").toString();
    } else {
        // This should never happen if userId is valid, but fallback anyway
        reply["status"]       = "ok";
        reply["username"]     = "";
        reply["email"]        = "";
        reply["name"]         = "";
        reply["department"]   = "";
        reply["title"]        = "";
        reply["description"]  = "";
    }

    return reply;
}

QJsonObject Server::handleDoctorUpdatePersonalInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "doctorUpdatePersonalInfo";

    QString token = request["token"].toString();
    int userId = checkDoctorToken(token);
    if (userId <= 0) {
        reply["status"] = "error";
        reply["message"] = "userId不能为空或无效";
        return reply;
    }

    QString name = request["name"].toString().trimmed();
    QString department = request["department"].toString().trimmed();
    QString title = request["title"].toString().trimmed();
    QString description = request["description"].toString().trimmed();

    QSqlQuery query;
    // Use INSERT with ON CONFLICT(id) DO UPDATE for upsert
    query.prepare("INSERT INTO doctor_personal_info (id, name, department, title, description) "
                  "VALUES (:id, :name, :department, :title, :description) "
                  "ON CONFLICT(id) DO UPDATE SET "
                  "    name = excluded.name, "
                  "    department = excluded.department, "
                  "    title = excluded.title, "
                  "    description = excluded.description;");
    query.bindValue(":id", userId);
    query.bindValue(":name", name);
    query.bindValue(":department", department);
    query.bindValue(":title", title);
    query.bindValue(":description", description);

    if (query.exec()) {
        reply["status"] = "ok";
        reply["message"] = "更新成功";
    } else {
        reply["status"] = "error";
        reply["message"] = "更新失败：" + query.lastError().text();
    }

    return reply;
}

QJsonObject Server::handleGetSessionInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "getSessionInfo";

    // Authenticate patient
    QString token = request["token"].toString();
    int patientId = checkToken(token);
    if (patientId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的token，请重新登录";
        return reply;
    }

    QSqlQuery query;
    query.prepare(R"(
    SELECT s.id AS sessionId, s.time,
           s.patientId AS patientId,
           s.doctorId AS doctorId,

           -- Patient info
           pi.name         AS patientName,
           pi.id_number    AS patientIdNumber,
           pi.phone_number AS patientPhone,

           -- Doctor info
           dpi.name        AS doctorName,
           dpi.department  AS doctorDepartment,
           dpi.title       AS doctorTitle,
           dpi.description AS doctorDescription

    FROM session s
    LEFT JOIN personal_info pi         ON s.patientId = pi.id
    LEFT JOIN doctor_personal_info dpi ON s.doctorId = dpi.id

    WHERE s.patientId = :patientId
)");
    query.bindValue(":patientId", patientId);

    if (query.exec() && query.next()) {
        reply["status"] = "ok";

        // Session info
        reply["sessionId"] = query.value("sessionId").toInt();
        reply["time"]      = query.value("time").toString();

        // Patient info
        QJsonObject patient;
        patient["id"]      = query.value("patientId").toInt();
        patient["name"]      = query.value("patientName").toString();
        patient["idNumber"]  = query.value("patientIdNumber").toString();
        patient["phone"]     = query.value("patientPhone").toString();
        reply["patient"] = patient;

        // Doctor info
        QJsonObject doctor;
        doctor["id"]         = query.value("doctorId").toInt();
        doctor["name"]        = query.value("doctorName").toString();
        doctor["department"]  = query.value("doctorDepartment").toString();
        doctor["title"]       = query.value("doctorTitle").toString();
        doctor["description"] = query.value("doctorDescription").toString();
        reply["doctor"] = doctor;

    } else {
        reply["status"] = "ok";
        reply["message"] = "未找到该会话";
    }

    return reply;
}

QJsonObject Server::handleDoctorGetSessionInfo(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "doctorGetSessionInfo";

    // Authenticate doctor
    QString token = request["token"].toString();
    int doctorId = checkDoctorToken(token);
    if (doctorId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的token，请重新登录";
        return reply;
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT s.id AS sessionId, s.time,
               s.patientId AS patientId,
               s.doctorId AS doctorId,
               pi.name        AS patientName,
               pi.id_number   AS patientIdNumber,
               pi.phone_number AS patientPhone,
               dpi.name       AS doctorName,
               dpi.department AS doctorDepartment,
               dpi.title      AS doctorTitle,
               dpi.description AS doctorDescription
        FROM session s
        LEFT JOIN personal_info pi          ON s.patientId = pi.id
        LEFT JOIN doctor_personal_info dpi  ON s.doctorId = dpi.id
        WHERE s.doctorId = :doctorId
        ORDER BY s.time DESC
    )");

    query.bindValue(":doctorId", doctorId);
    QJsonArray sessionsArray;

    if (query.exec()) {
        while (query.next()) {
            QJsonObject sessionObj;

            // Session info
            sessionObj["sessionId"] = query.value("sessionId").toInt();
            sessionObj["time"]      = query.value("time").toString();

            // Patient info
            QJsonObject patient;
            patient["id"]      = query.value("patientId").toInt();
            patient["name"]       = query.value("patientName").toString();
            patient["idNumber"]   = query.value("patientIdNumber").toString();
            patient["phone"]      = query.value("patientPhone").toString();
            sessionObj["patient"] = patient;

            // Doctor info
            QJsonObject doctor;
            doctor["id"]         = query.value("doctorId").toInt();
            doctor["name"]        = query.value("doctorName").toString();
            doctor["department"]  = query.value("doctorDepartment").toString();
            doctor["title"]       = query.value("doctorTitle").toString();
            doctor["description"] = query.value("doctorDescription").toString();
            sessionObj["doctor"] = doctor;

            sessionsArray.append(sessionObj);
        }
    }

    reply["status"] = "ok";
    reply["sessions"] = sessionsArray;

    return reply;
}

QJsonObject Server::handleEndSession(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "endSession";

    // Step 1: Authenticate doctor
    QString token = request["token"].toString();
    int doctorId = checkDoctorToken(token);
    if (doctorId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的token，请重新登录";
        return reply;
    }

    // Step 2: Validate sessionId
    int sessionId = request["sessionId"].toInt();
    if (sessionId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的sessionId";
        return reply;
    }

    QSqlQuery query;

    // Step 3: Check session exists and belongs to doctor
    query.prepare("SELECT * FROM session WHERE id = :sessionId AND doctorId = :doctorId");
    query.bindValue(":sessionId", sessionId);
    query.bindValue(":doctorId", doctorId);

    if (!query.exec() || !query.next()) {
        reply["status"] = "error";
        reply["message"] = "未找到该会话或无权限结束";
        return reply;
    }

    // Step 4: Move record to session_old
    int patientId = query.value("patientId").toInt();
    QString time = query.value("time").toString();

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO session_old (id, patientId, doctorId, time) VALUES (:id, :patientId, :doctorId, :time)");
    insertQuery.bindValue(":id", sessionId);  // keep same id
    insertQuery.bindValue(":patientId", patientId);
    insertQuery.bindValue(":doctorId", doctorId);
    insertQuery.bindValue(":time", time);

    if (!insertQuery.exec()) {
        reply["status"] = "error";
        reply["message"] = "移动会话到历史记录失败: " + insertQuery.lastError().text();
        return reply;
    }

    // Step 5: Delete from active session table
    query.prepare("DELETE FROM session WHERE id = :sessionId");
    query.bindValue(":sessionId", sessionId);

    if (!query.exec()) {
        reply["status"] = "error";
        reply["message"] = "删除会话失败: " + query.lastError().text();
        return reply;
    }

    reply["status"] = "ok";
    reply["message"] = "会诊已结束";
    reply["sessionId"] = sessionId;

    return reply;
}

QJsonObject Server::handleGetMessages(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "getMessages";

    // Step 1: Authenticate patient
    QString token = request["token"].toString();
    int patientId = checkToken(token);
    int doctorId;
    if (patientId <= 0) {
        doctorId = checkDoctorToken(token);
        if (doctorId <= 0) {
            reply["status"] = "error";
            reply["message"] = "无效的token，请重新登录";
            return reply;
        } else {
            patientId = request["partner_id"].toInt();
        }
    } else {
        doctorId = request["partner_id"].toInt();
    }

    if (doctorId <= 0 || patientId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的ID";
        return reply;
    }

    // Step 2: Query messages
    QSqlQuery query;
    query.prepare("SELECT sender_type, message, timestamp "
                  "FROM messages "
                  "WHERE doctor_id = :doctorId AND patient_id = :patientId "
                  "ORDER BY timestamp ASC");
    query.bindValue(":doctorId", doctorId);
    query.bindValue(":patientId", patientId);

    if (!query.exec()) {
        reply["status"] = "error";
        reply["message"] = "数据库查询失败: " + query.lastError().text();
        return reply;
    }

    // Step 3: Collect messages
    QJsonArray messages;
    while (query.next()) {
        QJsonObject msg;
        msg["sender_type"] = query.value("sender_type").toString();
        msg["message"] = query.value("message").toString();
        QDateTime dt = query.value("timestamp").toDateTime();
        msg["timestamp"] = dt.toMSecsSinceEpoch();
        messages.append(msg);
    }

    reply["status"] = "success";
    reply["messages"] = messages;
    return reply;
}

QJsonObject Server::handleSendMessage(const QJsonObject &request) {
    QJsonObject reply;
    reply["action"] = "sendMessage";

    QString token = request["token"].toString();
    QString messageText = request["message"].toString();
    if (messageText.isEmpty()) {
        reply["status"] = "error";
        reply["message"] = "消息不能为空";
        return reply;
    }

    int patientId = checkToken(token);
    int doctorId = -1;
    QString senderType;

    if (patientId > 0) {
        // Sender is patient
        doctorId = request["partner_id"].toInt();
        senderType = "patient";
    } else {
        // Try doctor
        doctorId = checkDoctorToken(token);
        if (doctorId > 0) {
            patientId = request["partner_id"].toInt();
            senderType = "doctor";
        } else {
            reply["status"] = "error";
            reply["message"] = "无效的token，请重新登录";
            return reply;
        }
    }

    if (doctorId <= 0 || patientId <= 0) {
        reply["status"] = "error";
        reply["message"] = "无效的ID";
        return reply;
    }

    // Insert message into database
    QSqlQuery query;
    query.prepare("INSERT INTO messages (doctor_id, patient_id, sender_type, message) "
                  "VALUES (:doctorId, :patientId, :senderType, :message)");
    query.bindValue(":doctorId", doctorId);
    query.bindValue(":patientId", patientId);
    query.bindValue(":senderType", senderType);
    query.bindValue(":message", messageText);

    if (!query.exec()) {
        reply["status"] = "error";
        reply["message"] = "消息发送失败: " + query.lastError().text();
        return reply;
    }

    reply["status"] = "success";
    reply["sender_type"] = senderType;
    reply["message"] = messageText;

    // Optional: Notify recipient immediately
    // If you have a connected socket map: e.g., patientSockets[patientId] or doctorSockets[doctorId]
    // you could do something like:
    // if (senderType == "doctor" && patientSockets.contains(patientId)) {
    //     patientSockets[patientId]->send(reply); // pseudo code
    // }
    // else if (senderType == "patient" && doctorSockets.contains(doctorId)) {
    //     doctorSockets[doctorId]->send(reply);
    // }

    return reply;
}


QString Server::generateToken() {
    QByteArray randomBytes = QUuid::createUuid().toByteArray();
    QString token = QCryptographicHash::hash(randomBytes, QCryptographicHash::Sha256).toHex();
    return token;
}  // member


QJsonObject Server::forwardDoctorAdviceRequest(const QJsonObject &actionRequest) {
    // actionRequest: {"action":"doctorAdvice", "request": {...}}
    if (actionRequest.value("action").toString() != "doctorAdvice")
        return QJsonObject{{"type", "error"}, {"message", "invalid_action"}};

    QJsonObject request = actionRequest.value("request").toObject();
    QJsonObject response = doctorAdviceServer.handleRequest(request);
    return response;
}
