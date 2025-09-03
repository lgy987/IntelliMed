#include "CaseNetworkManager.h"
#include "networkmanager.h"

CaseNetworkManager::CaseNetworkManager(QObject *parent)
    : QObject(parent)
{
}

CaseNetworkManager::~CaseNetworkManager()
{
}

CaseNetworkManager& CaseNetworkManager::instance()
{
    static CaseNetworkManager inst;
    return inst;
}

void CaseNetworkManager::sendRequest(const QJsonObject &request)
{
    // 创建统一的请求格式
    QJsonObject adaptedRequest;
    adaptedRequest.insert("action", "case_operation");
    adaptedRequest.insert("content", request);

    qDebug() << "发送请求:" << adaptedRequest;

    NetworkManager::instance().send(adaptedRequest);
}

void CaseNetworkManager::onReadyRead(const QJsonObject& rootObj)
{
    if (!rootObj.contains("action") || !rootObj.contains("content")) {
        emit errorOccurred("从服务器接收到无效的JSON数据");
        return;
    }

    QString action = rootObj.value("action").toString();
    QJsonObject content = rootObj.value("content").toObject();

    if (action == "case_operation") {
        emit dataReceived(content);
    } else {
        qDebug() << "Unknown action type:" << action;
        emit errorOccurred("未知的操作类型: " + action);
    }
}
