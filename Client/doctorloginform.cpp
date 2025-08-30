#include "doctorloginform.h"
#include "ui_doctorloginform.h"
#include "doctorsignupform.h"
#include "doctorhomepage.h"
#include "doctorsession.h"
#include "networkmanager.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

DoctorLoginForm::DoctorLoginForm(QWidget* liform, QWidget *parent)
    : QWidget(parent)
    , liform(liform)
    , dsuform(nullptr)
    , dhomepage(nullptr)
    , ui(new Ui::DoctorLoginForm)
{
    ui->setupUi(this);
    setupInputValidation(ui->usernameEdit, "^[A-Za-z0-9_]{1,50}$");
    setupInputValidation(ui->passwordEdit, "^[A-Za-z0-9!@#\\$%\\^&\\*]{1,50}$");

    connect(&NetworkManager::instance(), &NetworkManager::doctorLoginResponse, this, &DoctorLoginForm::handleLoginResponse);
    connect(ui->loginButton, &QPushButton::clicked, this, &DoctorLoginForm::handleLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &DoctorLoginForm::handleSignUpClicked);
    connect(ui->patientLoginButton, &QPushButton::clicked, this, &DoctorLoginForm::handlePatientLoginClicked);
    connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, &DoctorLoginForm::handleShowPasswordToggled);

    if (!DoctorSession::instance().username().isEmpty()) {
        ui->usernameEdit->setText(DoctorSession::instance().username());
    }

    if (!DoctorSession::instance().token().isEmpty()) {
        ui->rememberCheckBox->setChecked(true);
        // autologin
        QString username = DoctorSession::instance().username();
        QString token = DoctorSession::instance().token();
        NetworkManager::instance().sendDoctorTokenLogin(username, token);
    }
}

DoctorLoginForm::~DoctorLoginForm() { delete ui; }

void DoctorLoginForm::handleLoginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    bool remember    = ui->rememberCheckBox->isChecked();

    if (username.isEmpty() || password.isEmpty()) {
        ui->warningLabel->setText("请输入用户名和密码");
        return;
    } else {
        ui->warningLabel->clear();
    }

    NetworkManager::instance().sendDoctorLogin(username, password, remember);
}

void DoctorLoginForm::handleSignUpClicked()
{
    if(!dsuform) dsuform = new DoctorSignUpForm(this);
    dsuform->show();
    this->hide();
}

void DoctorLoginForm::handlePatientLoginClicked()
{
    if(!liform) {
        liform = new DoctorLoginForm(this);
    }

    liform->show();
    this->hide();
}

void DoctorLoginForm::handleLoginResponse(const QJsonObject &obj)
{
    if (obj["status"].toString() == "ok") {
        QString token = obj.value("token").toString();
        QString userId = obj.value("userId").toString();
        QString username = ui->usernameEdit->text();

        // Save session if "Remember Me" checked
        if (ui->rememberCheckBox->isChecked()) {
            DoctorSession::instance().enablePersistence(true);
        } else {
            DoctorSession::instance().enablePersistence(false);
        }

        // Only update session if login/token is valid
        DoctorSession::instance().setUser(username, userId, token);

        if(!dhomepage) dhomepage = new DoctorHomePage(this);
        dhomepage->show();
        this->hide();
    } else if (obj["status"].toString() == "retry") {
        DoctorSession::instance().clear();
    } else {
        QString msg = obj.value("message").toString("用户名或密码错误");
        ui->warningLabel->setText(msg);
    }
}

void DoctorLoginForm::handleShowPasswordToggled(bool checked)
{
    ui->passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}


void DoctorLoginForm::setupInputValidation(QLineEdit *lineEdit, const QString &pattern) {
    QRegularExpression regex(pattern);
    auto *validator = new QRegularExpressionValidator(regex, lineEdit);
    lineEdit->setValidator(validator);
}
