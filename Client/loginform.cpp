#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginForm)
    , socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // Connect socket signal
    connect(socket, &QTcpSocket::readyRead, this, &LoginForm::onReadyRead);

    // Connect buttons (Qt Designer will also auto-connect if names match)
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginForm::handleLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &LoginForm::handleSignUpClicked);

    // Auto-connect to server (change IP/port to your backend)
    socket->connectToHost("127.0.0.1", 12345);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::handleLoginClicked()
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 12345);
    }

    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    bool remember    = ui->rememberCheckBox->isChecked();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password.");
        return;
    }

    // JSON request
    QJsonObject req;
    req["action"]   = "login";
    req["username"] = username;
    req["password"] = password;
    req["remember"] = remember;

    QJsonDocument doc(req);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    socket->write(data + "\n"); // newline as delimiter
}

void LoginForm::handleSignUpClicked()
{
    QMessageBox::information(this, "Sign Up", "Sign up clicked! (not implemented yet)");
    // TODO: send signup request or open signup form
}

void LoginForm::onReadyRead()
{
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);

        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();

        if (obj.contains("status")) {
            QString status = obj["status"].toString();
            if (status == "ok") {
                QMessageBox::information(this, "Login", "Login successful!");
                // TODO: hide login form and open main app window
            } else {
                QString msg = obj.value("message").toString("Invalid username or password.");
                QMessageBox::warning(this, "Login", msg);
            }
        }
    }
}
