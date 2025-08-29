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
        reply["status"] = "error";
        reply["message"] = "Unknown action";
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
    query.bindValue(":password", password); // TODO: hash in production

    QJsonObject reply;
    if (query.exec() && query.next()) {
        QString token = generateToken();
        QString userId   = query.value("id").toString();

        QSqlQuery update;
        update.prepare("UPDATE users SET token = :token WHERE id = :id");
        update.bindValue(":token", token);
        update.bindValue(":id", userId.toInt());
        update.exec();

        reply["status"] = "ok";
        reply["token"] = token;
        reply["userId"] = query.value("id").toString();
        reply["username"] = username;
    } else {
        reply["status"] = "error";
        reply["message"] = "Invalid credentials";
    }
    return reply;
}

// Handle token-based login
QJsonObject Server::handleTokenLogin(const QJsonObject &request) {
    QString token = request["token"].toString();
    QSqlQuery query;
    QJsonObject reply;

    if (token.isEmpty()) {
        reply["status"] = "retry";
    } else {
        query.prepare("SELECT * FROM users WHERE token = :token");
        query.bindValue(":token", token);

        if (query.exec() && query.next()) {
            QString token = generateToken();
            QString username = query.value("username").toString();
            QString userId   = query.value("id").toString();

            QSqlQuery update;
            update.prepare("UPDATE users SET token = :token WHERE id = :id");
            update.bindValue(":token", token);
            update.bindValue(":id", userId.toInt());
            update.exec();

            reply["status"] = "ok";
            reply["token"] = token;
            reply["userId"] = query.value("id").toString();
            reply["username"] = query.value("username").toString();
        } else {
            reply["status"] = "retry";
        }
    }
    return reply;
}

// Handle user signup
QJsonObject Server::handleSignUp(const QJsonObject &request) {
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    QString email    = request["email"].toString();

    QJsonObject reply;
    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        reply["status"] = "error";
        reply["message"] = "All fields required";
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO users (username, password, email) VALUES (:username, :password, :email)");
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.bindValue(":email", email);

        if (query.exec()) {
            reply["status"] = "ok";
            reply["message"] = "Signup successful";
        } else {
            reply["status"] = "error";
            reply["message"] = query.lastError().text();
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

QString Server::generateToken() {
    QByteArray randomBytes = QUuid::createUuid().toByteArray();
    QString token = QCryptographicHash::hash(randomBytes, QCryptographicHash::Sha256).toHex();
    return token;
}
