#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class CaseNetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit CaseNetworkManager(QObject* parent = nullptr);
    ~CaseNetworkManager();

    // Static singleton instance
    static CaseNetworkManager& instance();

    // Wrap request and "send" it (no network code here)
    void sendRequest(const QJsonObject& request);

public slots:
    // Called when a reply is received (from some other component)
    void onReadyRead(const QJsonObject& rootObj);

signals:
    // Emitted when content is received
    void dataReceived(const QJsonObject& content);

    // Emitted on errors
    void errorOccurred(const QString& message);

private:
    // Disallow copying
    CaseNetworkManager(const CaseNetworkManager&) = delete;
    CaseNetworkManager& operator=(const CaseNetworkManager&) = delete;
};