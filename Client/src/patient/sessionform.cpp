#include "sessionform.h"
#include "networkmanager.h"
#include "ui_sessionform.h"
#include <QMessageBox>

SessionForm::SessionForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SessionForm)
    , m_partnerId(-1)
{
    ui->setupUi(this);

    // Connect NetworkManager signal
    connect(&NetworkManager::instance(), &NetworkManager::sessionInfoResponse,
            this, &SessionForm::onSessionInfoReceived);
    connect(ui->messageButton, &QPushButton::clicked,this, [this]() {
        emit startMessage(m_partnerId);
    });

    NetworkManager::instance().sendGetSessionInfo();
}

SessionForm::~SessionForm()
{
    delete ui;
}

void SessionForm::onSessionInfoReceived(const QJsonObject &reply)
{
    QString action = reply["action"].toString();

    if (action == "getSessionInfo" && reply["status"].toString() == "ok") {
        if (reply.contains("sessionId")) {
            // Set session info
            ui->sessionTimeEdit->setText(reply["time"].toString());

            // Patient info
            /**
            QJsonObject patient = reply["patient"].toObject();
            ui->patientNameEdit->setText(patient["name"].toString());
            ui->patientIdNumberEdit->setText(patient["idNumber"].toString());
            ui->patientPhoneEdit->setText(patient["phone"].toString());
            */

            // Doctor info
            QJsonObject doctor = reply["doctor"].toObject();
            ui->doctorNameEdit->setText(doctor["name"].toString());
            ui->doctorDepartmentEdit->setText(doctor["department"].toString());
            ui->doctorTitleEdit->setText(doctor["title"].toString());
            ui->doctorDescriptionEdit->setText(doctor["description"].toString());

            ui->cardsWidget->setVisible(true);
            ui->noSessionLabel->setVisible(false);
            m_partnerId = doctor["id"].toInt();
        } else {
            ui->cardsWidget->setVisible(false);
            ui->noSessionLabel->setVisible(true);
        }
    } else if (reply["status"].toString() == "error") {
        QMessageBox::warning(this, "错误", reply["message"].toString());
        ui->cardsWidget->setVisible(false);
        ui->noSessionLabel->setVisible(true);
    }
}

