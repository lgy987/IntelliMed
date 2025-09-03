// CaseServer.cpp
#include "CaseServer.h"

CaseServer::CaseServer(QObject* parent)
    : QObject(parent)
{
}

QJsonObject CaseServer::handleRequest(const QJsonObject& request)
{
    QJsonObject content;
    QString action = request.value("action").toString();

    if (action == "case_operation") {
        content = request.value("content").toObject();
    } else {
        content = request;
    }

    QString type = content.value("type").toString();
    QString virus = content.value("virus").toString();

    qDebug() << "=== Received request ===";
    qDebug() << "Action:" << action << "Type:" << type << "Virus:" << virus;

    QJsonObject responseContent;

    if (type == "patient_query") {
        QJsonArray cases = m_database.queryCasesByVirus(virus, false);
        qDebug() << "Found" << cases.size() << "patient records";

        responseContent.insert("status", "success");
        responseContent.insert("type", "patient_query_response");
        responseContent.insert("data", cases);
    } else if (type == "doctor_query") {
        QJsonArray cases = m_database.queryCasesByVirus(virus, true);
        qDebug() << "Found" << cases.size() << "doctor records";

        responseContent.insert("status", "success");
        responseContent.insert("type", "doctor_query_response");
        responseContent.insert("data", cases);
    } else {
        responseContent.insert("status", "error");
        responseContent.insert("message", "Unknown request type");
    }

    // Wrap response in standard format
    QJsonObject response;
    response.insert("action", "case_operation");
    response.insert("content", responseContent);

    qDebug() << "Response ready:" << response;
    return response;
}
