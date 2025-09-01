#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include "ai.h"

void Ai::callAI(const QString &userMessage) {
    auto *manager = new QNetworkAccessManager();

    // API endpoint
    QUrl url("https://open.bigmodel.cn/api/paas/v4/chat/completions");
    QNetworkRequest request(url);

    // Headers
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer 9195e9ac6f404632af0af3ea22564aa6.Kf1CWR7vBpz9K6jK"); //api key

    // JSON body
    QJsonObject message;
    message["role"] = "user";
    message["content"] = userMessage;

    QJsonArray messages;
    messages.append(message);

    QJsonObject body;
    body["model"] = "glm-4.5";
    body["messages"] = messages;
    body["temperature"] = 0.6;
    body["max_tokens"] = 1024;

    QJsonDocument doc(body);

    // POST request
    QNetworkReply *reply = manager->post(request, doc.toJson());

    // Handle async response
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response_data = reply->readAll();
            qDebug() << "Raw response:" << response_data;

            // Parse JSON
            QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
            QJsonObject obj = jsonResponse.object();

            // Extract answer
            QJsonArray choices = obj["choices"].toArray();
            if (!choices.isEmpty()) {
                QString answer = choices[0].toObject()["message"].toObject()["content"].toString();
                qDebug() << "AI Answer:" << answer;
            }
        } else {
            qDebug() << "Network Error:" << reply->errorString();
            qDebug() << "HTTP Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(); // 👈 print what server actually sent back
        }
        reply->deleteLater();
    });
}
