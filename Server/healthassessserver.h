#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "hadb.h"

class HealthAssessServer : public QObject {
    Q_OBJECT
public:
    explicit HealthAssessServer(QObject* parent = nullptr);
    QJsonObject handleRequest(const QJsonObject& req);

private:
    DatabaseManager m_db;
    QJsonObject handleInsert(const QJsonObject& req);
    QJsonObject handleGetAll();
    QJsonObject handleGetByName(const QString& name);
    QJsonObject handleGetByPhone(const QString& phone);
};
