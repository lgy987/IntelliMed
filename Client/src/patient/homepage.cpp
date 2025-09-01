#include "homepage.h"
#include "ui_homepage.h"
#include "personalinfoform.h"
#include "sessionform.h"
#include "message.h"
#include "session.h"
#include "../DoctorAdvice/patientwindow.h"
#include "../DoctorAdvice/medlink.h"
#include <QLabel>


HomePage::HomePage(QWidget *loginForm, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomePage)
    , loginForm(loginForm)
{
    ui->setupUi(this);
    ui->statusbar->hide();

    // 1. Create the container for the right side (top bar + stacked widget)
    QWidget *rightContainer = new QWidget(ui->centralwidget);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 2. Create the transparent top bar
    QWidget *topBar = new QWidget(rightContainer);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background-color: transparent;");

    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(10, 0, 10, 0);
    topBarLayout->setSpacing(10);

    // 3. Add a stretch to push content to the right
    topBarLayout->addStretch();

    // 4. Username label (normal font, slightly larger)
    QLabel *usernameLabel = new QLabel(Session::instance().username(), topBar);
    usernameLabel->setStyleSheet("color: #000000; font-size: 14px;");

    // 5. Logout button
    QPushButton *btnLogout = new QPushButton("登出", topBar);
    btnLogout->setFlat(true);
    btnLogout->setStyleSheet(R"(
    QPushButton {
        color: #000000;
        background-color: transparent;
        border: none;
        font-size: 14px;
    }
    QPushButton:hover {
        color: #2193F3;
    }
)");

    // 6. Add label and button to layout
    topBarLayout->addWidget(usernameLabel);
    topBarLayout->addSpacing(10); // small gap between label and button
    topBarLayout->addWidget(btnLogout);

    // 7. Add top bar and stacked widget to the vertical layout
    rightLayout->addWidget(topBar);
    rightLayout->addWidget(ui->stackedPages);

    // 8. Replace the original stacked widget in horizontal layout
    QLayoutItem *oldItem = ui->horizontalLayout_main->takeAt(1);
    if (oldItem) delete oldItem;
    ui->horizontalLayout_main->addWidget(rightContainer);

    setupButtons();
    setupPersonalInfoForm();
    setupSessionForm();
    setupMessage();
    setupAI();
    setupDoctorAdvice();
    QVBoxLayout* sidebarLayout = ui->verticalLayout_sidebar;
    if (sidebarLayout) {
        sidebarLayout->setContentsMargins(0, 0, 0, 0); // remove margins
        sidebarLayout->setSpacing(4);                 // spacing between buttons
    }

    connect(btnLogout, &QPushButton::clicked, this, &HomePage::handleLogout);
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

void HomePage::setupSessionForm()
{
    sform = new SessionForm(this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(1));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(sform);

    connect(sform, &SessionForm::startMessage, this, [=](int partnerId){
        msg->changePartner(partnerId);
        ui->btnCases->click();
    });
}

void HomePage::setupMessage()
{
    msg = new Message(-1, false, this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(2));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(msg);
}

void HomePage::setupAI()
{
    Message *AImsg = new Message(0, false, this);
    // Add it to the stacked page layout
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(3));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(AImsg);
}

void HomePage::setupDoctorAdvice()
{
    PatientWindow *pw = new PatientWindow(&MedLink::instance(), this);
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(4));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pw);
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
               ui->btnOrders, ui->btnHealth};

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

