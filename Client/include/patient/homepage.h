#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>
#include <QPushButton>

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
    void handlePersonalInfoClicked();
    void setupButtons();
    QList<QPushButton*> buttons;
    void setupPersonalInfoForm();
    void setupSessionForm();
};

#endif // HOMEPAGE_H
