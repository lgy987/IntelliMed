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

    connect(ui->signUpButton, &QPushButton::clicked, this, &SignUpForm::onSignUpButtonClicked);
    connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, &SignUpForm::onShowPasswordToggled);
    connect(ui->backToLoginButton, &QPushButton::clicked, this, &SignUpForm::onBackToLoginClicked);
    connect(network, &NetworkManager::signUpResponse, this, &SignUpForm::handleSignUpResponse);
}

SignUpForm::~SignUpForm()
{
    delete ui;
}

void SignUpForm::onSignUpButtonClicked()
{
    network->sendSignUp(
        ui->usernameEdit->text(),
        ui->passwordEdit->text(),
        ui->emailEdit->text()
        );
}

void SignUpForm::handleSignUpResponse(const QJsonObject &obj)
{
    if (obj["status"].toString() == "ok") {
        QMessageBox::information(this, "Sign Up", obj.value("message").toString("Sign up successful!"));
        this->close();
    } else {
        QMessageBox::warning(this, "Sign Up", obj.value("message").toString("Sign up failed."));
    }
}

void SignUpForm::onShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void SignUpForm::onBackToLoginClicked()
{
    this->deleteLater();
    if (loginForm) loginForm->show(); // show login form if it exists
}
