#include "doctorsignupform.h"
#include "ui_doctorsignupform.h"
#include "doctorloginform.h"
#include "networkmanager.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

DoctorSignUpForm::DoctorSignUpForm(QWidget *dloginForm, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoctorSignUpForm),
    dloginForm(dloginForm)
{
    ui->setupUi(this);
    DoctorLoginForm::setupInputValidation(ui->usernameEdit, "^[A-Za-z0-9_]{1,50}$");
    DoctorLoginForm::setupInputValidation(ui->passwordEdit, "^[A-Za-z0-9!@#\\$%\\^&\\*]{1,50}$");
    DoctorLoginForm::setupInputValidation(ui->emailEdit, R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,50}$)");

    connect(ui->signUpButton, &QPushButton::clicked, this, &DoctorSignUpForm::handleSignUpClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &DoctorSignUpForm::handleBackToLoginClicked);

    connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, &DoctorSignUpForm::handleShowPasswordToggled);
    connect(ui->passwordEdit, &QLineEdit::editingFinished, this, &DoctorSignUpForm::checkPasswordLength);
    connect(ui->confirmPasswordEdit, &QLineEdit::editingFinished, this, &DoctorSignUpForm::checkConfirmPassword);
    connect(ui->emailEdit, &QLineEdit::editingFinished, this, &DoctorSignUpForm::checkEmail);

    connect(&NetworkManager::instance(), &NetworkManager::doctorSignUpResponse, this, &DoctorSignUpForm::handleSignUpResponse);
}

DoctorSignUpForm::~DoctorSignUpForm()
{
    delete ui;
}

void DoctorSignUpForm::handleSignUpClicked()
{
    if(checkAllFields()) {
        QString username = ui->usernameEdit->text();
        QString password = ui->passwordEdit->text();
        QString email = ui->emailEdit->text();

        NetworkManager::instance().sendDoctorSignUp(username, password, email);
    }
}

void DoctorSignUpForm::handleSignUpResponse(const QJsonObject &obj)
{
    if (obj["status"].toString() == "ok") {
        QMessageBox::information(this, "Sign Up", obj.value("message").toString("注册成功"));
        if (dloginForm) dloginForm->show();
        this->hide();
    } else {
        QString msg = obj.value("message").toString("注册失败");
        ui->warningLabel->setText(msg);
    }
}

void DoctorSignUpForm::handleShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void DoctorSignUpForm::checkPasswordLength()
{
    QString password = ui->passwordEdit->text();
    if (password.length() < 8) {
        ui->warningLabel->setText("密码长度至少为 8 个字符");
    } else if (ui->warningLabel->text() == "密码长度至少为 8 个字符") {
        ui->warningLabel->clear();
    }
}

void DoctorSignUpForm::checkConfirmPassword()
{
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();
    if (password.length() < 8) {
        ui->warningLabel->setText("密码长度至少为 8 个字符");
    } else if (password != confirmPassword) {
        ui->warningLabel->setText("两次输入的密码不一致");
    } else if (ui->warningLabel->text() == "两次输入的密码不一致") {
        ui->warningLabel->clear();
    }
}

void DoctorSignUpForm::checkEmail()
{
    QString email = ui->emailEdit->text();
    if (!isValidEmail(email)) {
        ui->warningLabel->setText("邮箱格式不正确");
    } else if (ui->warningLabel->text() == "邮箱格式不正确") {
        ui->warningLabel->clear();
    }
}

void DoctorSignUpForm::handleBackToLoginClicked()
{
    this->hide();
    if (dloginForm) dloginForm->show();
}

bool DoctorSignUpForm::isValidEmail(const QString &email)
{
    static const QRegularExpression regex(
        R"((^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))"
        );
    return regex.match(email).hasMatch();
}

bool DoctorSignUpForm::checkAllFields(){
    QString username = ui->usernameEdit->text();
    QString email = ui->emailEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();
    if(username.isEmpty() || email.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()){
        ui->warningLabel->setText("请填写所有项目");
        return false;
    }
    if(!isValidEmail(email)){
        ui->warningLabel->setText("邮箱格式不正确");
        return false;
    }
    if (password.length() < 8) {
        ui->warningLabel->setText("密码长度至少为 8 个字符");
        return false;
    }
    if (password != confirmPassword) {
        ui->warningLabel->setText("两次输入的密码不一致");
        return false;
    }
    return true;
}
