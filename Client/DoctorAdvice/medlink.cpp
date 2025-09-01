#include "medlink.h"
#include <QJsonDocument>
#include "networkmanager.h"

MedLink::MedLink(QObject *parent) : QObject(parent) {
    /**connect(&sock_, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(&sock_, SIGNAL(connected()),  this, SLOT(onConnected()));
    connect(&sock_, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));*/
}

/**
void MedLink::connectToServer(const QString &host, quint16 port) {
    if (sock_.state() == QAbstractSocket::ConnectedState) return;
    sock_.connectToHost(host, port);
}

bool MedLink::isConnected() const { return sock_.state() == QAbstractSocket::ConnectedState; }
*/
MedLink& MedLink::instance() {
    static MedLink instance;
    return instance;
}

void MedLink::sendJson(const QJsonObject &obj) {
    QJsonObject wrapper;
    wrapper.insert("action", "doctoradvice");
    wrapper.insert("content", obj);
    NetworkManager::instance().send(wrapper);
    /*
    QJsonDocument doc(obj);
    QByteArray line = doc.toJson(QJsonDocument::Compact);
    line.append('\n');
    sock_.write(line);
    sock_.flush();
    */
}

void MedLink::onReadyRead(const QJsonObject &reply) {
    emit jsonReceived(reply);
    /**
    buffer_.append(sock_.readAll());
    while (true) {
        int idx = buffer_.indexOf('\n');
        if (idx < 0) break;
        QByteArray line = buffer_.left(idx);
        buffer_.remove(0, idx + 1);
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isObject()) emit jsonReceived(doc.object());
    }
    */
}

void MedLink::onConnected() { emit connected(); }
void MedLink::onError(QAbstractSocket::SocketError) { emit errorText(sock_.errorString()); }
