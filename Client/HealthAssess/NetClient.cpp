#include "NetClient.h"
#include "networkmanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

NetClient::NetClient(QObject* parent) : QObject(parent) {}

NetClient& NetClient::instance() {
    static NetClient inst;
    return inst;
}

void NetClient::send(const QJsonObject& obj) {
    QJsonObject wrapper;
    wrapper["action"] = "healthassess";
    wrapper["content"] = obj;
    NetworkManager::instance().send(wrapper);
}

// ---------------- Requests ----------------

void NetClient::insertPatient(const QJsonObject& p) {
    send(QJsonObject{
        {"cmd","insert"},
        {"name",p["name"]}, {"age",p["age"]},
        {"gender",p["gender"]}, {"height",p["height"]}, {"weight",p["weight"]},
        {"phone",p["phone"]}, {"bmi",p["bmi"]}, {"bmi_status",p["bmi_status"]},
        {"lifestyle_choices",p["lifestyle_choices"]}, {"health_score",p["health_score"]}
    });
}

void NetClient::getAll() {
    send(QJsonObject{{"cmd","get_all"}});
}

void NetClient::getByName(const QString& name) {
    send(QJsonObject{{"cmd","get_by_name"}, {"name", name}});
}

void NetClient::getByPhone(const QString& phone) {
    send(QJsonObject{{"cmd","get_by_phone"}, {"phone", phone}});
}

// ---------------- Response handler ----------------

void NetClient::onReadyRead(const QJsonObject &content) {
    qDebug().noquote() << "NetClient received reply:"
                       << QJsonDocument(content).toJson(QJsonDocument::Indented);

    QString cmd = content.value("cmd").toString();
    if (cmd == "insert") {
        emit patientInserted(content.value("ok").toBool());
    } else if (cmd == "get_all") {
        emit allPatientsReceived(content.value("data").toArray());
    } else if (cmd == "get_by_name") {
        emit patientByNameReceived(content.value("data").toObject());
    } else if (cmd == "get_by_phone") {
        emit patientByPhoneReceived(content.value("data").toObject());
    }
}
