#include "doctorpersonalinfoform.h"
#include "loginform.h"
#include "networkmanager.h"
#include "ui_doctorpersonalinfoform.h"
#include <QMessageBox>

DoctorPersonalInfoForm::DoctorPersonalInfoForm(QWidget *homepage, QWidget *parent)
    : QWidget(parent), homepage(homepage), ui(new Ui::DoctorPersonalInfoForm)
{
    ui->setupUi(this);
    //ui->usernameEdit->setEnabled(false);
    //ui->emailEdit->setEnabled(false);
    //LoginForm::setupInputValidation(ui->usernameEdit, "^[A-Za-z0-9_]{1,50}$");
    //LoginForm::setupInputValidation(ui->emailEdit, R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,50}$)");
    LoginForm::setupInputValidation(ui->nameEdit, "^[\u4e00-\u9fa5·]{1,50}$");
    LoginForm::setupInputValidation(ui->departmentEdit, "^[\u4e00-\u9fa5·A-Za-z0-9._%+-]{1,50}$");
    LoginForm::setupInputValidation(ui->titleEdit, "^[\u4e00-\u9fa5·A-Za-z0-9._%+-]{1,50}$");
    //LoginForm::setupInputValidation(ui->descriptionEdit, "^[A-Za-z0-9._%+-\u4e00-\u9fa5·]{1,1000}$");

    // initial state
    NetworkManager::instance().sendDoctorGetPersonalInfo();
    setEditMode(false);
    ui->saveButton->hide();
    ui->cancelButton->hide();

    connect(ui->editButton, &QPushButton::clicked, this, &DoctorPersonalInfoForm::onEditClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &DoctorPersonalInfoForm::onSaveClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DoctorPersonalInfoForm::onCancelClicked);
    connect(&NetworkManager::instance(), &NetworkManager::doctorPersonalInfoResponse,
            this, &DoctorPersonalInfoForm::onPersonalInfoReceived);
}

DoctorPersonalInfoForm::~DoctorPersonalInfoForm()
{
    delete ui;
}

void DoctorPersonalInfoForm::setEditMode(bool enable) {
    ui->nameEdit->setEnabled(enable);
    ui->departmentEdit->setEnabled(enable);
    ui->titleEdit->setEnabled(enable);
    ui->descriptionEdit->setEnabled(enable);
}

void DoctorPersonalInfoForm::onEditClicked() {
    setEditMode(true);
    ui->editButton->hide();
    ui->saveButton->show();
    ui->cancelButton->show();
}

void DoctorPersonalInfoForm::onSaveClicked() {
    // save changes to database or object
    setEditMode(false);
    QJsonObject data;
    data["name"]         = ui->nameEdit->text();
    data["department"] = ui->departmentEdit->text();
    data["title"]    = ui->titleEdit->text();
    data["description"] = ui->descriptionEdit->toPlainText();
    NetworkManager::instance().sendDoctorUpdatePersonalInfo(data);
}

void DoctorPersonalInfoForm::onCancelClicked() {
    // revert changes if needed
    NetworkManager::instance().sendDoctorGetPersonalInfo();

    setEditMode(false);
    ui->editButton->show();
    ui->saveButton->hide();
    ui->cancelButton->hide();
}

void DoctorPersonalInfoForm::onPersonalInfoReceived(const QJsonObject &reply)
{
    QString action = reply["action"].toString();
    if (action == "doctorGetPersonalInfo" && reply["status"] == "ok") {
        ui->usernameEdit->setText(reply["username"].toString());
        ui->emailEdit->setText(reply["email"].toString());
        ui->nameEdit->setText(reply["name"].toString());
        ui->departmentEdit->setText(reply["department"].toString());
        ui->titleEdit->setText(reply["title"].toString());
        ui->descriptionEdit->setPlainText(reply["description"].toString());
    }
    else if (action == "doctorUpdatePersonalInfo") {
        if (reply["status"] == "ok") {
            QMessageBox::information(this, "成功", "个人信息已更新");
            setEditMode(false);
            ui->saveButton->hide();
            ui->cancelButton->hide();
            ui->editButton->show();
        } else {
            QMessageBox::warning(this, "错误", reply["message"].toString());
        }
    }
}
