#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>

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
};

#endif // HOMEPAGE_H
