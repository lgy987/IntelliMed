#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "doctoradviceserver.h"

class Server : public QTcpServer {
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();
    QJsonObject handleAction(QTcpSocket* client, const QJsonObject &request);

private:
    //QList<QTcpSocket*> clients;
    QHash<QString, QTcpSocket*> clients;

    void sendResponse(QTcpSocket *client, const QJsonObject &reply);
    QString generateToken();

    // User handling
    QJsonObject handleLogin(QTcpSocket *client, const QJsonObject &request);
    QJsonObject handleTokenLogin(QTcpSocket *client, const QJsonObject &request);
    QJsonObject handleSignUp(const QJsonObject &request);
    QJsonObject createSessionForUser(const int &userId, const QString &username, QTcpSocket* sock);
    QJsonObject handleGetPersonalInfo(const QJsonObject &request);
    QJsonObject handleUpdatePersonalInfo(const QJsonObject &request);
    QJsonObject handleGetSessionInfo(const QJsonObject &request);
    void handleAISendMessage(QTcpSocket* client, const QJsonObject &request);
    int checkToken(const QString &token);
    QString getPatientToken(int id);
    void callAI(QTcpSocket *client, int senderPatientId, const QString &userMessage);

    // Doctor handling
    QJsonObject handleDoctorLogin(QTcpSocket *client, const QJsonObject &request);
    QJsonObject handleDoctorTokenLogin(QTcpSocket *client, const QJsonObject &request);
    QJsonObject handleDoctorSignUp(const QJsonObject &request);
    QJsonObject createSessionForDoctor(const int &userId, const QString &username, QTcpSocket* sock);
    QJsonObject handleDoctorGetPersonalInfo(const QJsonObject &request);
    QJsonObject handleDoctorUpdatePersonalInfo(const QJsonObject &request);
    QJsonObject handleDoctorGetSessionInfo(const QJsonObject &request);
    QJsonObject handleEndSession(const QJsonObject &request);
    int checkDoctorToken(const QString &token);
    QString getDoctorToken(int id);

    // Messaging
    QJsonObject handleGetMessages(const QJsonObject &request);
    QJsonObject handleSendMessage(const QJsonObject &request);

    //DoctorAdvice
    DoctorAdviceServer doctorAdviceServer;
    QJsonObject forwardDoctorAdviceRequest(const QJsonObject &actionRequest);
};
