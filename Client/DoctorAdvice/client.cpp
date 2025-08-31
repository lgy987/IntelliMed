#include "client.h"
#include <QJsonDocument>

Client::Client(QObject *parent) : QObject(parent) {
    connect(&sock_, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&sock_, SIGNAL(connected()),  this, SLOT(onConnected()));
    connect(&sock_, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Client::connectToServer(const QString &host, quint16 port) {
    if (sock_.state() == QAbstractSocket::ConnectedState) return;
    sock_.connectToHost(host, port);
}

bool Client::isConnected() const {
    return sock_.state() == QAbstractSocket::ConnectedState;
}

void Client::sendJson(const QJsonObject &obj) {
    QJsonDocument doc(obj);
    QByteArray line = doc.toJson(QJsonDocument::Compact);
    line.append('\n');
    sock_.write(line);
    sock_.flush();
}

void Client::onReadyRead() {
    buffer_.append(sock_.readAll());
    while (true) {
        int idx = buffer_.indexOf('\n');
        if (idx < 0) break;
        QByteArray line = buffer_.left(idx);
        buffer_.remove(0, idx + 1);

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isObject()) emit jsonReceived(doc.object());
    }
}

void Client::onConnected() { emit connected(); }
void Client::onError(QAbstractSocket::SocketError) { emit errorText(sock_.errorString()); }
