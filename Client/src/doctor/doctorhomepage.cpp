#include "doctorhomepage.h"
#include "ui_doctorhomepage.h"
#include "doctorsessionform.h"
#include "doctorpersonalinfoform.h"
#include "doctorsession.h"
#include "../DoctorAdvice/doctorwindow.h"
#include "../HealthAssess/HealthAssess.h"
#include "../Cases/DoctorView.h"

DoctorHomePage::DoctorHomePage(QWidget *dloginForm, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorHomePage)
    , dloginForm(dloginForm)
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
    QLabel *usernameLabel = new QLabel(DoctorSession::instance().username(), topBar);
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
        color: #06b6d4;
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
    setupDoctorAdvice();
    setupHealthAssess();
    setupCases();
    QVBoxLayout *sidebarLayout = ui->verticalLayout_sidebar;
    if (sidebarLayout) {
        sidebarLayout->setContentsMargins(0, 0, 0, 0); // remove margins
        sidebarLayout->setSpacing(4);                 // spacing between buttons
    }

    connect(btnLogout, &QPushButton::clicked, this, &DoctorHomePage::handleLogout);
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

void DoctorHomePage::setupHealthAssess()
{
    HealthAssess *ha = new HealthAssess(this);
    QVBoxLayout* layout = new QVBoxLayout(ui->stackedPages->widget(3));
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(ha);
}

void DoctorHomePage::setupCases()
{
    QStackedWidget* stacked = ui->stackedPages;

    // Ensure page at index 5 exists
    QWidget* page = nullptr;
    if (stacked->count() > 5) {
        page = stacked->widget(5);
    } else {
        page = new QWidget();
        stacked->insertWidget(5, page);
    }

    // Give it a layout if it doesn't have one
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(page->layout());
    if (!layout) {
        layout = new QVBoxLayout(page);
        layout->setContentsMargins(0,0,0,0);
    }

    // Add PatientView
    DoctorView* pv = new DoctorView(page);
    layout->addWidget(pv);
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
            background-color: #06b6d4; /* modern cyan */
            text-align: left;
        }
    )";

    buttons = { ui->btnUser, ui->btnAppointment, ui->btnMessage,
               ui->btnOrders, ui->btnHealth, ui->btnCases};

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
