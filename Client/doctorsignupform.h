#ifndef DOCTORSIGNUPFORM_H
#define DOCTORSIGNUPFORM_H

#include <QWidget>

namespace Ui {
class DoctorSignUpForm;
}

class DoctorSignUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorSignUpForm(QWidget *dloginForm, QWidget *parent = nullptr);
    ~DoctorSignUpForm();

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
    Ui::DoctorSignUpForm *ui;
    QWidget *dloginForm;
};

#endif // SIGNUPFORM_H
