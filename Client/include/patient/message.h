#pragma once

#include <QWidget>
#include <QJsonObject>
#include <QVBoxLayout>

namespace Ui {
class Message;
}

class Message : public QWidget
{
    Q_OBJECT

public:
    explicit Message(int partnerId, bool isDoctor, QWidget *parent = nullptr);
    ~Message();
    void changePartner(int partnerId);
    void appendMessage(const QString &senderType, const QString &text, const QString &timestamp);

private slots:
    void onSendButtonClicked();
    void onGetMessages(const QJsonObject &obj);
    void onSendMessageAck(const QJsonObject &obj);

private:
    Ui::Message *ui;
    int m_partnerId;
    bool m_isDoctor;

    void loadMessages();
    void refresh();
};
