#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginForm; }
QT_END_NAMESPACE

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void handleLoginClicked();
    void handleSignUpClicked();
    void onReadyRead();

private:
    Ui::LoginForm *ui;
    QTcpSocket *socket;
};

#endif
