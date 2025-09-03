// CaseServer.h
#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "CaseDatabase.h"  // your database interface

class CaseServer : public QObject
{
    Q_OBJECT
public:
    explicit CaseServer(QObject* parent = nullptr);

    // Main function: takes request and returns response
    QJsonObject handleRequest(const QJsonObject& request);

private:
    CaseDatabase m_database; // your database interface
};
