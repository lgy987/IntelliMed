#include "message.h"
#include "ui_message.h"
#include "networkmanager.h"
#include <QLabel>
#include <QScrollBar>
#include <QDateTime>
#include <QJsonArray>
#include <QDateTime>

Message::Message(int partnerId, bool isDoctor, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Message),
    m_partnerId(partnerId),
    m_isDoctor(isDoctor)
{
    ui->setupUi(this);

    // Connect send button
    connect(ui->sendButton, &QPushButton::clicked, this, &Message::onSendButtonClicked);

    // Connect network signals
    connect(&NetworkManager::instance(), &NetworkManager::getMessagesResponse,
            this, &Message::onGetMessages);
    connect(&NetworkManager::instance(), &NetworkManager::sendMessageResponse,
            this, &Message::onSendMessageAck);

    // Load existing messages
    refresh();
}

Message::~Message()
{
    delete ui;
}

void Message::changePartner(int partnerId){
    if (partnerId <= 0) m_partnerId = -1;
    else m_partnerId = partnerId;
    refresh();
}

// Load messages from server
void Message::loadMessages()
{
    if(m_isDoctor) NetworkManager::instance().doctorGetMessages(m_partnerId);
    else NetworkManager::instance().getMessages(m_partnerId);
}

// Append a single message bubble
void Message::appendMessage(const QString &senderType, const QString &text, const QString &timestamp)
{
    QLabel *msgLabel = new QLabel(this);
    msgLabel->setWordWrap(true);
    msgLabel->setText(QString("[%1] %2").arg(timestamp, text));

    if ((m_isDoctor && senderType == "doctor") || (!m_isDoctor && senderType == "patient")) {
        msgLabel->setStyleSheet("background:#d1d5db; color:#111827; padding:8px; border-radius:8px; max-width:300px;");
        msgLabel->setAlignment(Qt::AlignRight);
    } else {
        msgLabel->setStyleSheet("background:#3b82f6; color:white; padding:8px; border-radius:8px; max-width:300px;");
        msgLabel->setAlignment(Qt::AlignLeft);
    }

    ui->messagesLayout->insertWidget(ui->messagesLayout->count() - 1, msgLabel);

    // Auto scroll to bottom
    QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void Message::refresh(){
    if (m_partnerId == -1) {
        ui->warningLabel->setVisible(true);
        ui->cardWidget->setVisible(false);
    } else {
        loadMessages();
        ui->warningLabel->setVisible(false);
        ui->cardWidget->setVisible(true);
    }
}

// Send button clicked
void Message::onSendButtonClicked()
{
    QString text = ui->messageEdit->text().trimmed();
    if (text.isEmpty()) return;

    if(m_isDoctor) NetworkManager::instance().doctorSendMessage(m_partnerId, text);
    else NetworkManager::instance().sendMessage(m_partnerId, text);

    ui->messageEdit->clear();
}

// Handle getMessages response
void Message::onGetMessages(const QJsonObject &obj)
{
    if (obj.value("status").toString() != "success") return;

    QJsonArray messages = obj.value("messages").toArray();
    for (int i = 0; i < messages.size(); ++i) {
        QJsonObject msgObj = messages.at(i).toObject();

        qint64 ms = msgObj.value("timestamp").toVariant().toLongLong();
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(ms); // UTC
        QString localTimeStr = dt.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");

        appendMessage(msgObj.value("sender_type").toString(),
                      msgObj.value("message").toString(),
                      localTimeStr);
    }
}

// Handle sendMessage ack

void Message::onSendMessageAck(const QJsonObject &obj)
{
    if (obj.value("status").toString() != "success") return;

    // Generate local timestamp
    QString localTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    appendMessage(obj.value("sender_type").toString(),
                  obj.value("message").toString(),
                  localTimestamp);
}
