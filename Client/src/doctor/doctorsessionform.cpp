#include "doctorsessionform.h"
#include "networkmanager.h"
#include "ui_doctorsessionform.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

DoctorSessionForm::DoctorSessionForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorSessionForm)
    , m_partnerId(-1)
    , currentIndex(0)
{
    ui->setupUi(this);

    // Disable editing of all fields
    ui->sessionTimeEdit->setEnabled(false);
    ui->patientNameEdit->setEnabled(false);
    ui->patientIdNumberEdit->setEnabled(false);
    ui->patientPhoneEdit->setEnabled(false);
    ui->doctorNameEdit->setEnabled(false);
    ui->doctorDepartmentEdit->setEnabled(false);
    ui->doctorTitleEdit->setEnabled(false);
    ui->doctorDescriptionEdit->setEnabled(false);

    // Connect NetworkManager signal
    connect(&NetworkManager::instance(), &NetworkManager::doctorSessionInfoResponse,
            this, &DoctorSessionForm::onDoctorSessionReceived);
    connect(&NetworkManager::instance(), &NetworkManager::doctorEndSessionResponse,
            this, &DoctorSessionForm::handleEndSessionResponse,
            Qt::UniqueConnection);
    // Connect navigation buttons
    connect(ui->prevButton, &QPushButton::clicked, this, &DoctorSessionForm::showPreviousSession);
    connect(ui->nextButton, &QPushButton::clicked, this, &DoctorSessionForm::showNextSession);
    connect(ui->endSessionButton, &QPushButton::clicked, this, &DoctorSessionForm::endCurrentSession);
    connect(ui->messageButton, &QPushButton::clicked,this, [this]() {
        emit startMessage(m_partnerId);
    });

    // Request all sessions for the doctor
    NetworkManager::instance().sendDoctorGetSessionInfo();
}

DoctorSessionForm::~DoctorSessionForm()
{
    delete ui;
}

void DoctorSessionForm::onDoctorSessionReceived(const QJsonObject &reply)
{
    if (reply["status"].toString() == "ok") {
        // Load sessions array
        QJsonArray sessionsArray = reply["sessions"].toArray();
        sessions.clear();
        for (int i = 0; i < sessionsArray.size(); ++i) {
            QJsonValue val = sessionsArray.at(i);
            if (val.isObject()) {
                sessions.append(val.toObject());
            }
        }

        updateSessionUI();

    } else if (reply["status"].toString() == "error") {
        QMessageBox::warning(this, "错误", reply["message"].toString());
    }
}

void DoctorSessionForm::updateSessionUI()
{
    if (!sessions.isEmpty()) {
        currentIndex = qBound(0, currentIndex, sessions.size()-1);
        showSession(currentIndex);
        ui->cardsWidget->setVisible(true);
        ui->noSessionsLabel->setVisible(false);
    } else {
        ui->cardsWidget->setVisible(false);
        ui->noSessionsLabel->setVisible(true);
    }
}

void DoctorSessionForm::showSession(int index)
{
    if (index < 0 || index >= sessions.size()) return;

    const QJsonObject &session = sessions[index];

    // Session info
    ui->sessionTimeEdit->setText(session["time"].toString());

    // Patient info
    QJsonObject patient = session["patient"].toObject();
    ui->patientNameEdit->setText(patient["name"].toString());
    ui->patientIdNumberEdit->setText(patient["idNumber"].toString());
    ui->patientPhoneEdit->setText(patient["phone"].toString());

    // Doctor info
    QJsonObject doctor = session["doctor"].toObject();
    ui->doctorNameEdit->setText(doctor["name"].toString());
    ui->doctorDepartmentEdit->setText(doctor["department"].toString());
    ui->doctorTitleEdit->setText(doctor["title"].toString());
    ui->doctorDescriptionEdit->setText(doctor["description"].toString());

    // Enable/disable navigation buttons
    ui->prevButton->setEnabled(index > 0);
    ui->nextButton->setEnabled(index < sessions.size() - 1);

    m_partnerId = patient["id"].toInt();
}

void DoctorSessionForm::showPreviousSession()
{
    if (currentIndex > 0) {
        currentIndex--;
        showSession(currentIndex);
    }
}

void DoctorSessionForm::showNextSession()
{
    if (currentIndex < sessions.size() - 1) {
        currentIndex++;
        showSession(currentIndex);
    }
}

void DoctorSessionForm::endCurrentSession()
{
    if (sessions.isEmpty() || currentIndex < 0 || currentIndex >= sessions.size())
        return;

    int sessionId = sessions[currentIndex]["sessionId"].toInt();
    NetworkManager::instance().sendDoctorEndSession(sessionId);
}

void DoctorSessionForm::handleEndSessionResponse(const QJsonObject &reply)
{
    if (reply["status"].toString() != "ok") {
        QMessageBox::warning(this, "失败", reply["message"].toString());
        return;
    }

    if (!reply.contains("sessionId") || !reply["sessionId"].isDouble()) {
        // Bad sessionId, refresh the whole list
        NetworkManager::instance().sendDoctorGetSessionInfo();
        return;
    }

    int deletedSessionId = reply["sessionId"].toInt();

    // Find the session with the returned sessionId
    bool removed = false;
    for (int i = 0; i < sessions.size(); ++i) {
        if (sessions[i].contains("sessionId") && sessions[i]["sessionId"].toInt() == deletedSessionId) {
            sessions.removeAt(i);
            removed = true;

            // Adjust currentIndex if needed
            if (currentIndex >= sessions.size())
                currentIndex = sessions.size() - 1;
            updateSessionUI();
            showSession(currentIndex);
            break;
        }
    }

    if (removed) {
        QMessageBox::information(this, "成功", "会诊已结束");
    } else {
        // Session not found locally, refresh the whole list
        NetworkManager::instance().sendDoctorGetSessionInfo();
    }
}
