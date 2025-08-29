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
    connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, &SignUpForm::handleShowPasswordToggled);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &SignUpForm::handleBackToLoginClicked);
    connect(network, &NetworkManager::signUpResponse, this, &SignUpForm::handleSignUpResponse);
}

SignUpForm::~SignUpForm()
{
    delete ui;
}

void SignUpForm::handleSignUpClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        ui->warningLabel->setText("Please enter all fields.");
        return;
    } else {
        ui->warningLabel->clear();
    }

    network->sendSignUp(username, password, email);
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

void SignUpForm::handleBackToLoginClicked()
{
    this->deleteLater();
    if (loginForm) loginForm->show();
}
