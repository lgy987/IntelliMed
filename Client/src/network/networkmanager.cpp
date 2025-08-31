#include "networkmanager.h"
#include "session.h"
#include "doctorsession.h"
#include <QJsonDocument>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
}

NetworkManager::~NetworkManager() { }

NetworkManager& NetworkManager::instance() {
    static NetworkManager instance;
    return instance;
}

void NetworkManager::connectToServer(const QString &host, quint16 port)
{
    if (socket->state() != QAbstractSocket::ConnectedState)
        socket->connectToHost(host, port);
}

void NetworkManager::disconnectFromServer()
{
    if (socket->state() == QAbstractSocket::ConnectedState ||
        socket->state() == QAbstractSocket::ConnectingState) {
        socket->disconnectFromHost();
    }

    // If the server doesn't respond, force close
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->close();
    }
}

void NetworkManager::sendLogin(const QString &username, const QString &password, bool remember)
{
    QJsonObject req;
    req["action"]   = "login";
    req["username"] = username;
    req["password"] = password;
    req["remember"] = remember;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendTokenLogin(const QString &username, const QString &token)
{
    QJsonObject req;
    req["action"] = "tokenLogin";
    req["username"] = username;
    req["token"] = token;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendSignUp(const QString &username, const QString &password, const QString &email)
{
    QJsonObject req;
    req["action"]   = "signup";
    req["username"] = username;
    req["password"] = password;
    req["email"]    = email;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendGetPersonalInfo()
{
    QJsonObject req;
    req["action"] = "getPersonalInfo";
    req["token"]  = Session::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendUpdatePersonalInfo(const QJsonObject &data)
{
    QJsonObject req = data;
    req["action"] = "updatePersonalInfo";
    req["token"]  = Session::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorLogin(const QString &username, const QString &password, bool remember)
{
    QJsonObject req;
    req["action"]   = "doctorLogin";
    req["username"] = username;
    req["password"] = password;
    req["remember"] = remember;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorTokenLogin(const QString &username, const QString &token)
{
    QJsonObject req;
    req["action"] = "doctorTokenLogin";
    req["username"] = username;
    req["token"] = token;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorSignUp(const QString &username, const QString &password, const QString &email)
{
    QJsonObject req;
    req["action"]   = "doctorSignup";
    req["username"] = username;
    req["password"] = password;
    req["email"]    = email;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorGetPersonalInfo()
{
    QJsonObject req;
    req["action"] = "doctorGetPersonalInfo";
    req["token"]  = DoctorSession::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorUpdatePersonalInfo(const QJsonObject &data)
{
    QJsonObject req = data;
    req["action"] = "doctorUpdatePersonalInfo";
    req["token"]  = DoctorSession::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendGetSessionInfo()
{

    QJsonObject req;
    req["action"] = "getSessionInfo";
    req["token"]  = Session::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorGetSessionInfo()
{

    QJsonObject req;
    req["action"] = "doctorGetSessionInfo";
    req["token"]  = DoctorSession::instance().token();

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::sendDoctorEndSession(int sessionId)
{
    QJsonObject req;
    req["action"] = "endSession";
    req["token"] = DoctorSession::instance().token();
    req["sessionId"] = sessionId;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void NetworkManager::onReadyRead()
{
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();

        QString action = obj.value("action").toString();
        qDebug().noquote() << QJsonDocument(obj).toJson(QJsonDocument::Indented);
        if (action == "signup") {
            emit signUpResponse(obj);
        } else if (action == "login") {
            emit loginResponse(obj);
        } else if (action == "getPersonalInfo" || action == "updatePersonalInfo") {
            emit personalInfoResponse(obj);
        } else if (action == "doctorSignup") {
            emit doctorSignUpResponse(obj);
        } else if (action == "doctorLogin") {
            emit doctorLoginResponse(obj);
        } else if (action == "doctorGetPersonalInfo" || action == "doctorUpdatePersonalInfo") {
            emit doctorPersonalInfoResponse(obj);
        } else if (action == "getSessionInfo") {
            emit sessionInfoResponse(obj);
        } else if (action == "doctorGetSessionInfo") {
            emit doctorSessionInfoResponse(obj);
        } else if (action == "endSession") {
            emit doctorEndSessionResponse(obj);
        }
    }
}


