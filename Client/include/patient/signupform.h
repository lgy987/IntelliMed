#ifndef SIGNUPFORM_H
#define SIGNUPFORM_H

#include <QWidget>

namespace Ui {
class SignUpForm;
}

class SignUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpForm(QWidget *loginForm, QWidget *parent = nullptr);
    ~SignUpForm();

private slots:
    void handleSignUpClicked();
    void handleSignUpResponse(const QJsonObject &obj);
    void handleShowPasswordToggled(bool checked);
    void handleBackToLoginClicked();
    void checkPasswordLength();
    void checkConfirmPassword();
    void checkEmail();
    bool checkAllFields();
    bool isValidEmail(const QString &email);

private:
    Ui::SignUpForm *ui;
    QWidget *loginForm;
};

#endif // SIGNUPFORM_H
