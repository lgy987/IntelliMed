#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

namespace Ui { class LoginForm; }

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();
    static void setupInputValidation(QLineEdit *lineEdit, const QString &pattern);

private slots:
    void handleLoginClicked();
    void handleSignUpClicked();
    void handleLoginResponse(const QJsonObject &obj);
    void handleShowPasswordToggled(bool checked);

private:
    Ui::LoginForm *ui;
};

#endif // LOGINFORM_H
