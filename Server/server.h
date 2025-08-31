#pragma once
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>
#include "doctoradviceserver.h"

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    bool start(quint16 port);

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();

private:
    QWebSocketServer *webSocketServer;
    QHash<QString, QWebSocket*> clients;  // token/username -> socket

    void sendResponse(QWebSocket *client, const QJsonObject &reply);
    QString generateToken();

    // === Handlers ===
    QJsonObject handleAction(const QJsonObject &request);

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

    DoctorAdviceServer doctorAdviceServer;
    QJsonObject forwardDoctorAdviceRequest(const QJsonObject &actionRequest);
};
