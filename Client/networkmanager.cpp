#include "networkmanager.h"
#include <QJsonDocument>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
}

NetworkManager::~NetworkManager() { }

void NetworkManager::connectToServer(const QString &host, quint16 port)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        socket->connectToHost(host, port);
}

void NetworkManager::sendLogin(const QString &username, const QString &password, bool remember)
{
    QJsonObject req;
    req["action"]   = "login";
    req["username"] = username;
    req["password"] = password;
    req["remember"] = remember;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendTokenLogin(const QString &username, const QString &token)
{
    QJsonObject req;
    req["action"] = "tokenLogin";
    req["username"] = username;
    req["token"] = token;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::onReadyRead()
{
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);

        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();
        emit loginResponse(obj);
    }
}
