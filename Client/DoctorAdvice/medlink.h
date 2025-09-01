#ifndef MEDLINK_H
#define MEDLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class MedLink : public QObject {
    Q_OBJECT
public:
    explicit MedLink(QObject *parent = 0);
    static MedLink& instance();
    void connectToServer(const QString &host="127.0.0.1", quint16 port=5555);
    bool isConnected() const;
    void sendJson(const QJsonObject &obj);
    void onReadyRead(const QJsonObject &reply);

signals:
    void jsonReceived(const QJsonObject &obj);
    void connected();
    void errorText(const QString &msg);

private slots:
    void onConnected();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket sock_;
    QByteArray buffer_;
};

#endif
