#ifndef DOCTORHOMEPAGE_H
#define DOCTORHOMEPAGE_H

#include <QMainWindow>
#include <QPushButton>

#include "doctorsessionform.h"
#include "message.h"

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
    DoctorSessionForm *dsform = nullptr;
    Message *msg = nullptr;
    Ui::DoctorHomePage *ui;
    QWidget *dloginForm;
    void handlePersonalInfoClicked();
    void setupButtons();
    QList<QPushButton*> buttons;
    void setupPersonalInfoForm();
    void setupSessionForm();
    void setupMessage();
};

#endif // HOMEPAGE_H
