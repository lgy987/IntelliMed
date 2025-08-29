#include "loginform.h"
#include "ui_loginform.h"
#include "signupform.h"
#include "homepage.h"
#include "session.h"

#include <QMessageBox>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
    , network(new NetworkManager(this))
{
    ui->setupUi(this);

    network->connectToServer();

    connect(network, &NetworkManager::loginResponse, this, &LoginForm::handleLoginResponse);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginForm::handleLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &LoginForm::handleSignUpClicked);

    if (!Session::instance().username().isEmpty()) {
        ui->usernameEdit->setText(Session::instance().username());
    }

    if (!Session::instance().token().isEmpty()) {
        ui->rememberCheckBox->setChecked(true);
        // autologin
        QString username = Session::instance().username();
        QString token = Session::instance().token();
        network->sendTokenLogin(username, token);
    }
}

LoginForm::~LoginForm() { delete ui; }

void LoginForm::handleLoginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    bool remember    = ui->rememberCheckBox->isChecked();

    if (username.isEmpty() || password.isEmpty()) {
        ui->warningLabel->setText("Please enter username and password.");
        return;
    } else {
        ui->warningLabel->clear();
    }

    network->sendLogin(username, password, remember);
}

void LoginForm::handleSignUpClicked()
{
    SignUpForm *form = new SignUpForm(nullptr);
    form->show();
    this->hide();
}

void LoginForm::handleLoginResponse(const QJsonObject &obj)
{
    if (obj["status"].toString() == "ok") {
        QString token = obj.value("token").toString();
        QString userId = obj.value("userId").toString();
        QString username = ui->usernameEdit->text();

        // Save session if "Remember Me" checked
        if (ui->rememberCheckBox->isChecked()) {
            Session::instance().enablePersistence(true);
        } else {
            Session::instance().enablePersistence(false);
        }

        // Only update session if login/token is valid
        Session::instance().setUser(username, userId, token);

        HomePage *home = new HomePage(this);
        home->show();
        this->hide();
    } else if (obj["status"].toString() == "retry") {
        Session::instance().clear();
        QString msg = obj.value("message").toString("Invalid token");
        ui->warningLabel->setText(msg);
    } else {
        QString msg = obj.value("message").toString("Invalid username or password.");
        ui->warningLabel->setText(msg);
    }
}
