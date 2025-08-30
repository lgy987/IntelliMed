#include "homepage.h"
#include "ui_homepage.h"
#include "personalinfoform.h"
#include "session.h"

HomePage::HomePage(QWidget *loginForm, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomePage)
    , loginForm(loginForm)
{
    ui->setupUi(this);
    setupButtons();
    setupPersonalInfoForm();
    QVBoxLayout* sidebarLayout = ui->verticalLayout_sidebar;
    if (sidebarLayout) {
        sidebarLayout->setContentsMargins(0, 0, 0, 0); // remove margins
        sidebarLayout->setSpacing(4);                 // spacing between buttons
    }

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

    this->hide();
}

void HomePage::setupPersonalInfoForm()
{
    PersonalInfoForm* piform = new PersonalInfoForm(this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(0));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(piform);
}

void HomePage::setupButtons()
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
            background-color: #2193F3; /* modern blue */
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
