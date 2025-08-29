#ifndef SIGNUPFORM_H
#define SIGNUPFORM_H

#include <QWidget>
#include "networkmanager.h"

namespace Ui {
class SignUpForm;
}

class SignUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpForm(QWidget *loginForm, NetworkManager *network, QWidget *parent = nullptr);
    ~SignUpForm();

private slots:
    void onSignUpButtonClicked();
    void handleSignUpResponse(const QJsonObject &obj);
    void onShowPasswordToggled(bool checked);
    void onBackToLoginClicked();

private:
    Ui::SignUpForm *ui;
    QWidget *loginForm;
    NetworkManager *network;
};

#endif // SIGNUPFORM_H
