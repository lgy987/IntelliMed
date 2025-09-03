#include "healthassessserver.h"
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>

static QJsonArray mapListToJson(const QList<QMap<QString, QVariant>>& list) {
    QJsonArray arr;
    for (const auto& row : list) {
        QJsonObject obj;
        for (auto it = row.begin(); it != row.end(); ++it) {
            obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
        }
        arr.append(obj);
    }
    return arr;
}

HealthAssessServer::HealthAssessServer(QObject* parent) : QObject(parent) {
    if (!m_db.initializeDatabase()) {
        qWarning() << "DB init failed, but server will continue.";
    }
}

// Main entry point for your main server to call
QJsonObject HealthAssessServer::handleRequest(const QJsonObject& req) {
    const QString cmd = req.value("cmd").toString();
    if (cmd == "insert")        return handleInsert(req);
    if (cmd == "get_all")       return handleGetAll();
    if (cmd == "get_by_name")   return handleGetByName(req.value("name").toString());
    if (cmd == "get_by_phone")  return handleGetByPhone(req.value("phone").toString());
    return QJsonObject{{"ok", false}, {"error", "unknown_cmd"}};
}

QJsonObject HealthAssessServer::handleInsert(const QJsonObject& r) {
    bool ok = m_db.insertPatientData(
        r.value("name").toString(),
        r.value("age").toInt(),
        r.value("gender").toString(),
        r.value("height").toDouble(),
        r.value("weight").toDouble(),
        r.value("phone").toString(),
        r.value("bmi").toDouble(),
        r.value("bmi_status").toString(),
        r.value("lifestyle_choices").toString(),
        r.value("health_score").toInt()
        );
    return QJsonObject{{"ok", ok}};
}

QJsonObject HealthAssessServer::handleGetAll() {
    auto list = m_db.getPatientData();
    return QJsonObject{{"ok", true}, {"data", mapListToJson(list)}};
}

QJsonObject HealthAssessServer::handleGetByName(const QString& name) {
    auto row = m_db.getPatientByName(name);
    QJsonObject obj;
    for (auto it = row.begin(); it != row.end(); ++it)
        obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
    return QJsonObject{{"ok", true}, {"data", obj}};
}

QJsonObject HealthAssessServer::handleGetByPhone(const QString& phone) {
    auto row = m_db.getPatientByPhone(phone);
    QJsonObject obj;
    for (auto it = row.begin(); it != row.end(); ++it)
        obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
    return QJsonObject{{"ok", true}, {"data", obj}};
}
