#ifndef DOCTORLOGINFORM_H
#define DOCTORLOGINFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

namespace Ui { class DoctorLoginForm; }

class DoctorLoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorLoginForm(QWidget *liform, QWidget *parent = nullptr);
    ~DoctorLoginForm();
    static void setupInputValidation(QLineEdit *lineEdit, const QString &pattern);

private slots:
    void handleLoginClicked();
    void handleSignUpClicked();
    void handleLoginResponse(const QJsonObject &obj);
    void handleShowPasswordToggled(bool checked);
    void handlePatientLoginClicked();

private:
    QWidget *liform;
    QWidget *dsuform;
    QWidget *dhomepage;
    Ui::DoctorLoginForm *ui;
};

#endif // LOGINFORM_H
