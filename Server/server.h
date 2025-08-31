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
    void sendResponse(QTcpSocket *client, const QJsonObject &reply);
    QString generateToken();

    QJsonObject handleLogin(const QJsonObject &request);
    QJsonObject handleTokenLogin(const QJsonObject &request);
    QJsonObject handleSignUp(const QJsonObject &request);
    QJsonObject createSessionForUser(const int &userId, const QString &username);
    QJsonObject handleGetPersonalInfo(const QJsonObject &request);
    QJsonObject handleUpdatePersonalInfo(const QJsonObject &request);
    QJsonObject handleGetSessionInfo(const QJsonObject &request);
    int checkToken(const QString &token);

    QJsonObject handleDoctorLogin(const QJsonObject &request);
    QJsonObject handleDoctorTokenLogin(const QJsonObject &request);
    QJsonObject handleDoctorSignUp(const QJsonObject &request);
    QJsonObject createSessionForDoctor(const int &userId, const QString &username);
    QJsonObject handleDoctorGetPersonalInfo(const QJsonObject &request);
    QJsonObject handleDoctorUpdatePersonalInfo(const QJsonObject &request);
    QJsonObject handleDoctorGetSessionInfo(const QJsonObject &request);
    QJsonObject handleEndSession(const QJsonObject &request);
    int checkDoctorToken(const QString &token);

    QJsonObject handleGetMessages(const QJsonObject &request);
    QJsonObject handleSendMessage(const QJsonObject &request);

};
