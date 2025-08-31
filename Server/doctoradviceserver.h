#ifndef DOCTORADVICESERVER_H
#define DOCTORADVICESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QMap>
#include "dadb.h"  // your database class

struct Conn {
    QTcpSocket *sock;
    QByteArray buffer;
};

class DoctorAdviceServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit DoctorAdviceServer(QObject *parent = nullptr);

    bool start();

    // new helper: directly process JSON from main server
    QJsonObject handleRequest(const QJsonObject &request);

protected:
    void incomingConnection(qintptr handle) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    void handleJson(QTcpSocket *s, const QJsonObject &obj);
    void sendJson(QTcpSocket *s, const QJsonObject &obj);

    QMap<QTcpSocket*, Conn*> conns_;
    Database db_;  // your database handler
};

#endif // DOCTORADVICESERVER_H
