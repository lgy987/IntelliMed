#include "signupform.h"
#include "ui_signupform.h"
#include "loginform.h"
#include "networkmanager.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

SignUpForm::SignUpForm(QWidget *loginForm, NetworkManager *network, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignUpForm),
    loginForm(loginForm),
    network(network)
{
    ui->setupUi(this);
    LoginForm::setupInputValidation(ui->usernameEdit, "^[A-Za-z0-9_]{1,50}$");
    LoginForm::setupInputValidation(ui->passwordEdit, "^[A-Za-z0-9!@#\\$%\\^&\\*]{1,50}$");
    LoginForm::setupInputValidation(ui->emailEdit, R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,50}$)");

    connect(ui->signUpButton, &QPushButton::clicked, this, &SignUpForm::handleSignUpClicked);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &SignUpForm::handleBackToLoginClicked);

    connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, &SignUpForm::handleShowPasswordToggled);
    connect(ui->passwordEdit, &QLineEdit::editingFinished, this, &SignUpForm::checkPasswordLength);
    connect(ui->confirmPasswordEdit, &QLineEdit::editingFinished, this, &SignUpForm::checkConfirmPassword);
    connect(ui->emailEdit, &QLineEdit::editingFinished, this, &SignUpForm::checkEmail);

    connect(network, &NetworkManager::signUpResponse, this, &SignUpForm::handleSignUpResponse);
}

SignUpForm::~SignUpForm()
{
    delete ui;
}

void SignUpForm::handleSignUpClicked()
{
    if(checkAllFields()) {
        QString username = ui->usernameEdit->text();
        QString password = ui->passwordEdit->text();
        QString email = ui->emailEdit->text();

        network->sendSignUp(username, password, email);
    }
}

void SignUpForm::handleSignUpResponse(const QJsonObject &obj)
{
    if (obj["status"].toString() == "ok") {
        QMessageBox::information(this, "Sign Up", obj.value("message").toString("注册成功"));
        this->deleteLater();
        if (loginForm) loginForm->show();
    } else {
        QString msg = obj.value("message").toString("注册失败");
        ui->warningLabel->setText(msg);
    }
}

void SignUpForm::handleShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void SignUpForm::checkPasswordLength()
{
    QString password = ui->passwordEdit->text();
    if (password.length() < 8) {
        ui->warningLabel->setText("密码长度至少为 8 个字符");
    } else if (ui->warningLabel->text() == "密码长度至少为 8 个字符") {
        ui->warningLabel->clear();
    }
}

void SignUpForm::checkConfirmPassword()
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

void SignUpForm::checkEmail()
{
    QString email = ui->emailEdit->text();
    if (!isValidEmail(email)) {
        ui->warningLabel->setText("邮箱格式不正确");
    } else if (ui->warningLabel->text() == "邮箱格式不正确") {
        ui->warningLabel->clear();
    }
}

void SignUpForm::handleBackToLoginClicked()
{
    this->deleteLater();
    if (loginForm) loginForm->show();
}

bool SignUpForm::isValidEmail(const QString &email)
{
    static const QRegularExpression regex(
        R"((^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))"
        );
    return regex.match(email).hasMatch();
}

bool SignUpForm::checkAllFields(){
    QString username = ui->usernameEdit->text();
    QString email = ui->emailEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();
    if(username.isEmpty() || email.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()){
        ui->warningLabel->setText("请填写所有输入项");
        return false;
    }
    if(!isValidEmail(email)){
        ui->warningLabel->setText("邮箱格式不正确");
        return false;
    }
    if (password.length() < 8) {
        ui->warningLabel->setText("密码长度至少为 8 个字符");
    }
    if (password != confirmPassword) {
        ui->warningLabel->setText("两次输入的密码不一致");
    }
    return true;
}
