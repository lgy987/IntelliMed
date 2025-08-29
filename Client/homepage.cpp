#include "homepage.h"
#include "ui_homepage.h"
#include "session.h"

HomePage::HomePage(QWidget *loginForm, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomePage)
    , loginForm(loginForm)
{
    ui->setupUi(this);

    connect(ui->btnLogout, &QPushButton::clicked, this, &HomePage::handleLogout);
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::handleLogout()
{
    // Clear session
    Session::instance().clear();

    // Show the login form again
    if (loginForm) {
        loginForm->show();
    }

    this->deleteLater();
}
