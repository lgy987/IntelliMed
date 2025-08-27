#include "server.h"
#include <QDebug>

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

        if (obj["action"].toString() == "login") {
            QString username = obj["username"].toString();
            QString password = obj["password"].toString();

            // --- Dummy login check ---
            QJsonObject reply;
            if (username == "admin" && password == "1234") {
                reply["status"] = "ok";
            } else {
                reply["status"] = "error";
                reply["message"] = "Invalid credentials";
            }

            QJsonDocument responseDoc(reply);
            client->write(responseDoc.toJson(QJsonDocument::Compact) + "\n");
        }
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
