#include "signupform.h"
#include "ui_signupform.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

SignUpForm::SignUpForm(QTcpSocket *socket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignUpForm),
    socket(socket)
{
    ui->setupUi(this);

    connect(ui->signUpButton, &QPushButton::clicked, this, &SignUpForm::on_signUpButton_clicked);
    connect(socket, &QTcpSocket::readyRead, this, &SignUpForm::onReadyRead);
}

SignUpForm::~SignUpForm()
{
    delete ui;
}

void SignUpForm::on_signUpButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString email    = ui->emailEdit->text();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    // JSON signup request
    QJsonObject req;
    req["action"]   = "signup";
    req["username"] = username;
    req["password"] = password;
    req["email"]    = email;

    QJsonDocument doc(req);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void SignUpForm::onReadyRead()
{
    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        QJsonDocument doc = QJsonDocument::fromJson(line);

        if (!doc.isObject()) continue;
        QJsonObject obj = doc.object();

        if (obj.contains("status")) {
            QString status = obj["status"].toString();
            if (status == "ok") {
                QMessageBox::information(this, "Sign Up", obj.value("message").toString("Sign up successful!"));
                this->close();
            } else {
                QString msg = obj.value("message").toString("Sign up failed.");
                QMessageBox::warning(this, "Sign Up", msg);
            }
        }
    }
}
