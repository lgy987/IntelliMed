#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>
#include <QPushButton>
#include "sessionform.h"
#include "message.h"

namespace Ui {
class HomePage;
}

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *loginForm, QWidget *parent = nullptr);
    ~HomePage();
    void handleLogout();

private:
    Ui::HomePage *ui;
    QWidget *loginForm;

    SessionForm *sform = nullptr;
    Message *msg = nullptr;

    void handlePersonalInfoClicked();
    void setupButtons();
    QList<QPushButton*> buttons;
    void setupPersonalInfoForm();
    void setupSessionForm();
    void setupMessage();
    void setupAI();
};

#endif // HOMEPAGE_H
