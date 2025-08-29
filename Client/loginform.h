#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "networkmanager.h"

namespace Ui { class LoginForm; }

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void handleLoginClicked();
    void handleSignUpClicked();
    void handleLoginResponse(const QJsonObject &obj);

private:
    Ui::LoginForm *ui;
    NetworkManager *network;
};

#endif // LOGINFORM_H
