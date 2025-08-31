#ifndef DOCTORHOMEPAGE_H
#define DOCTORHOMEPAGE_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class DoctorHomePage;
}

class DoctorHomePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit DoctorHomePage(QWidget *dloginForm, QWidget *parent = nullptr);
    ~DoctorHomePage();
    void handleLogout();

private:
    Ui::DoctorHomePage *ui;
    QWidget *dloginForm;
    void handlePersonalInfoClicked();
    void setupButtons();
    QList<QPushButton*> buttons;
    void setupPersonalInfoForm();
    void setupSessionForm();
};

#endif // HOMEPAGE_H
