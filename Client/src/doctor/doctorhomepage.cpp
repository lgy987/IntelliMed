#include "doctorhomepage.h"
#include "ui_doctorhomepage.h"
#include "doctorsessionform.h"
#include "doctorpersonalinfoform.h"
#include "doctorsession.h"
#include "../DoctorAdvice/doctorwindow.h"

DoctorHomePage::DoctorHomePage(QWidget *dloginForm, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorHomePage)
    , dloginForm(dloginForm)
{
    ui->setupUi(this);
    setupButtons();
    setupPersonalInfoForm();
    setupSessionForm();
    setupMessage();
    setupDoctorAdvice();
    QVBoxLayout *sidebarLayout = ui->verticalLayout_sidebar;
    if (sidebarLayout) {
        sidebarLayout->setContentsMargins(0, 0, 0, 0); // remove margins
        sidebarLayout->setSpacing(4);                 // spacing between buttons
    }

    connect(ui->btnLogout, &QPushButton::clicked, this, &DoctorHomePage::handleLogout);
}

DoctorHomePage::~DoctorHomePage()
{
    delete ui;
}

void DoctorHomePage::handleLogout()
{
    // Clear session
    DoctorSession::instance().clear();

    // Show the login form again
    if (dloginForm) {
        dloginForm->show();
    }

    this->hide();
}

void DoctorHomePage::setupPersonalInfoForm()
{
    DoctorPersonalInfoForm* dpiform = new DoctorPersonalInfoForm(this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(0));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(dpiform);
}

void DoctorHomePage::setupSessionForm()
{
    dsform = new DoctorSessionForm(this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(1));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(dsform);

    connect(dsform, &DoctorSessionForm::startMessage, this, [=](int partnerId){
        msg->changePartner(partnerId);
        ui->btnCases->click();
    });
}

void DoctorHomePage::setupMessage()
{
    msg = new Message(-1, true, this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(2));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(msg);
}

void DoctorHomePage::setupDoctorAdvice()
{
    DoctorWindow *pw = new DoctorWindow(&MedLink::instance(), this);
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(4));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pw);
}

void DoctorHomePage::setupButtons()
{
    // Set all buttons initially as inactive (gray / transparent)
    QString inactiveStyle = R"(
        QPushButton {
            font-size: 15px;
            color: #333;
            padding: 10px;
            margin: 0px;
            border-radius: 5px;
            background-color: rgba(0,0,0,0);
            text-align: left;
        }
        QPushButton:hover {
            background-color: rgba(0,0,0,0.1);
        }
    )";

    QString activeStyle = R"(
        QPushButton {
            font-size: 15px;
            color: white;
            padding: 10px;
            margin: 0px;
            border-radius: 5px;
            background-color: #06b6d4; /* modern blue */
            text-align: left;
        }
    )";

    buttons = { ui->btnUser, ui->btnAppointment, ui->btnCases,
               ui->btnOrders, ui->btnHealth, ui->btnLogout };

    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton* btn = buttons[i];
        btn->setStyleSheet(inactiveStyle);

        connect(btn, &QPushButton::clicked, this, [this, i, activeStyle, inactiveStyle](){
            ui->stackedPages->setCurrentIndex(i);
            for (int j = 0; j < buttons.size(); ++j)
                buttons[j]->setStyleSheet(j == i ? activeStyle : inactiveStyle);
        });
    }

    // Activate first page by default
    buttons[0]->click();
}
