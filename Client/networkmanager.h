#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    static NetworkManager& instance();

    void connectToServer(const QString &host = "127.0.0.1", quint16 port = 12345);
    void disconnectFromServer();

    void sendLogin(const QString &username, const QString &password, bool remember);
    void sendTokenLogin(const QString &username, const QString &token);
    void sendSignUp(const QString &username, const QString &password, const QString &email);
    void sendGetPersonalInfo();
    void sendUpdatePersonalInfo(const QJsonObject &data);
    void sendDoctorLogin(const QString &username, const QString &password, bool remember);
    void sendDoctorTokenLogin(const QString &username, const QString &token);
    void sendDoctorSignUp(const QString &username, const QString &password, const QString &email);
    void sendDoctorGetPersonalInfo();
    void sendDoctorUpdatePersonalInfo(const QJsonObject &data);

signals:
    void loginResponse(const QJsonObject &obj);
    void signUpResponse(const QJsonObject &obj);
    void personalInfoResponse(const QJsonObject &obj);
    void doctorLoginResponse(const QJsonObject &obj);
    void doctorSignUpResponse(const QJsonObject &obj);
    void doctorPersonalInfoResponse(const QJsonObject &obj);

private slots:
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // NETWORKMANAGER_H
