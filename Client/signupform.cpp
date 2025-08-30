#include "signupform.h"
#include "ui_signupform.h"
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
        QMessageBox::information(this, "Sign Up", obj.value("message").toString("Sign up successful!"));
        this->deleteLater();
        if (loginForm) loginForm->show();
    } else {
        QString msg = obj.value("message").toString("Sign up failed.");
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
        ui->warningLabel->setText("Password must be at least 8 characters long.");
    } else if (ui->warningLabel->text() == "Password must be at least 8 characters long.") {
        ui->warningLabel->clear();
    }
}

void SignUpForm::checkConfirmPassword()
{
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();
    if (password.length() < 8) {
        ui->warningLabel->setText("Password must be at least 8 characters long.");
    } else if (password != confirmPassword) {
        ui->warningLabel->setText("Passwords do not match.");
    } else if (ui->warningLabel->text() == "Passwords do not match.") {
        ui->warningLabel->clear();
    }
}

void SignUpForm::checkEmail()
{
    QString email = ui->emailEdit->text();
    if (!isValidEmail(email)) {
        ui->warningLabel->setText("Invalid email format.");
    } else if (ui->warningLabel->text() == "Invalid email format.") {
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
        ui->warningLabel->setText("Please fill all fields.");
        return false;
    }
    if(!isValidEmail(email)){
        ui->warningLabel->setText("Invalid email format.");
        return false;
    }
    if (password.length() < 8) {
        ui->warningLabel->setText("Password must be at least 8 characters long.");
    }
    if (password != confirmPassword) {
        ui->warningLabel->setText("Passwords do not match.");
    }
    return true;
}
