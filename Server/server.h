#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class Server : public QTcpServer {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();
    QJsonObject handleAction(const QJsonObject &request);

private:
    QList<QTcpSocket*> clients;
    QJsonObject handleLogin(const QJsonObject &request);
    QJsonObject handleTokenLogin(const QJsonObject &request);
    QJsonObject handleSignUp(const QJsonObject &request);
    void sendResponse(QTcpSocket *client, const QJsonObject &reply);
    QJsonObject createSessionForUser(const int &userId, const QString &username);
    QString generateToken();
    QJsonObject handleGetPersonalInfo(const QJsonObject &request);
    QJsonObject handleUpdatePersonalInfo(const QJsonObject &request);
    int checkToken(const QString &token);
};
