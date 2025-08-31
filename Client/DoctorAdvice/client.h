#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

    void connectToServer(const QString &host="127.0.0.1", quint16 port=5555);
    bool isConnected() const;

    void sendJson(const QJsonObject &obj);

signals:
    void jsonReceived(const QJsonObject &obj);
    void connected();
    void errorText(const QString &msg);

private slots:
    void onReadyRead();
    void onConnected();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket sock_;
    QByteArray buffer_;
};

#endif
