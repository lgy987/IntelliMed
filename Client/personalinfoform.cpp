#include "personalinfoform.h"
#include "loginform.h"
#include "networkmanager.h"
#include "ui_personalinfoform.h"
#include <QMessageBox>

PersonalInfoForm::PersonalInfoForm(QWidget *homepage, QWidget *parent)
    : QWidget(parent), homepage(homepage), ui(new Ui::PersonalInfoForm)
{
    ui->setupUi(this);
    ui->usernameEdit->setEnabled(false);
    ui->emailEdit->setEnabled(false);
    //LoginForm::setupInputValidation(ui->usernameEdit, "^[A-Za-z0-9_]{1,50}$");
    //LoginForm::setupInputValidation(ui->emailEdit, R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,50}$)");
    LoginForm::setupInputValidation(ui->nameEdit, "^[\u4e00-\u9fa5·]{1,50}$");
    LoginForm::setupInputValidation(ui->idNumberEdit, "^[0-9]{1,18}$");
    LoginForm::setupInputValidation(ui->phoneEdit, "^[0-9]{1,10}$");

    // initial state
    NetworkManager::instance().sendGetPersonalInfo();
    setEditMode(false);
    ui->saveButton->hide();
    ui->cancelButton->hide();

    connect(ui->editButton, &QPushButton::clicked, this, &PersonalInfoForm::onEditClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &PersonalInfoForm::onSaveClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &PersonalInfoForm::onCancelClicked);
    connect(&NetworkManager::instance(), &NetworkManager::personalInfoResponse,
            this, &PersonalInfoForm::onPersonalInfoReceived);
}

PersonalInfoForm::~PersonalInfoForm()
{
    delete ui;
}

void PersonalInfoForm::setEditMode(bool enable) {
    ui->nameEdit->setEnabled(enable);
    ui->idNumberEdit->setEnabled(enable);
    ui->phoneEdit->setEnabled(enable);
}

void PersonalInfoForm::onEditClicked() {
    setEditMode(true);
    ui->editButton->hide();
    ui->saveButton->show();
    ui->cancelButton->show();
}

void PersonalInfoForm::onSaveClicked() {
    // save changes to database or object
    setEditMode(false);
    QJsonObject data;
    data["name"]         = ui->nameEdit->text();
    data["id_number"]    = ui->idNumberEdit->text();
    data["phone_number"] = ui->phoneEdit->text();
    NetworkManager::instance().sendUpdatePersonalInfo(data);
}

void PersonalInfoForm::onCancelClicked() {
    // revert changes if needed
    NetworkManager::instance().sendGetPersonalInfo();

    setEditMode(false);
    ui->editButton->show();
    ui->saveButton->hide();
    ui->cancelButton->hide();
}

void PersonalInfoForm::onPersonalInfoReceived(const QJsonObject &reply)
{
    QString action = reply["action"].toString();
    if (action == "getPersonalInfo" && reply["status"] == "ok") {
        ui->usernameEdit->setText(reply["username"].toString());
        ui->emailEdit->setText(reply["email"].toString());
        ui->nameEdit->setText(reply["name"].toString());
        ui->idNumberEdit->setText(reply["id_number"].toString());
        ui->phoneEdit->setText(reply["phone_number"].toString());
    }
    else if (action == "updatePersonalInfo") {
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
