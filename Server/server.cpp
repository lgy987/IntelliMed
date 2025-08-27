#include "server.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

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
        QJsonObject obj = doc.object();

        QString action = obj["action"].toString();
        QJsonObject reply;

        QSqlQuery query;

        if (action == "login") {
            QString username = obj["username"].toString();
            QString password = obj["password"].toString();

            query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
            query.bindValue(":username", username);
            query.bindValue(":password", password); // TODO: use hashed passwords in production

            if (query.exec() && query.next()) {
                reply["status"] = "ok";
            } else {
                reply["status"] = "error";
                reply["message"] = "Invalid credentials";
            }

        } else if (action == "signup") {
            QString username = obj["username"].toString();
            QString password = obj["password"].toString();
            QString email    = obj["email"].toString();

            if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
                reply["status"] = "error";
                reply["message"] = "All fields required";
            } else {
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

        } else {
            reply["status"] = "error";
            reply["message"] = "Unknown action";
        }

        QJsonDocument responseDoc(reply);
        client->write(responseDoc.toJson(QJsonDocument::Compact) + "\n");
    }
}

void Server::onDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
        qDebug() << "Client disconnected.";
    }
}
