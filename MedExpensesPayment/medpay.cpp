#include "medpay.h"
#include "ui_medpay.h"
#include "unpaid.h"
#include "paymenthistory.h"
#include "e_invoice.h"
#include "database.h"
#include <QMessageBox>

MedPay::MedPay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedPay)
    , database(nullptr)
{
    ui->setupUi(this);

    // 设置数据库
    setupDatabase();

    // 创建子窗口，传递数据库实例
    unpaidWindow = new Unpaid(database);
    historyWindow = new PaymentHistory(database);
    invoiceWindow = new E_Invoice(database);

    // 连接数据库数据变化信号，自动刷新所有子窗口
    if (database) {
        connect(database, &Database::dataChanged, this, [this]() {
            // 刷新所有子窗口的数据
            if (unpaidWindow) {
                unpaidWindow->refreshData();
            }
            if (historyWindow) {
                historyWindow->refreshData();
            }
            if (invoiceWindow) {
                invoiceWindow->refreshData();
            }
            // 刷新主界面数据
            refreshData();
        });
    }

    // 连接按钮信号
    connect(ui->unpaidButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        unpaidWindow->show();
        unpaidWindow->raise();
        unpaidWindow->activateWindow();
    });
    connect(ui->historyButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        historyWindow->show();
        historyWindow->raise();
        historyWindow->activateWindow();
    });
    connect(ui->invoiceButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        invoiceWindow->show();
        invoiceWindow->raise();
        invoiceWindow->activateWindow();
    });

    // 连接返回信号
    connect(unpaidWindow, &Unpaid::backRequested, this, [this]() {
        unpaidWindow->hide();
        this->show();
        this->raise();
        this->activateWindow();
        refreshData();
    });
    connect(historyWindow, &PaymentHistory::backRequested, this, [this]() {
        historyWindow->hide();
        this->show();
        this->raise();
        this->activateWindow();
        refreshData();
    });
    connect(invoiceWindow, &E_Invoice::backRequested, this, [this]() {
        invoiceWindow->hide();
        this->show();
        this->raise();
        this->activateWindow();
        refreshData();
    });

    // 加载用户信息
    loadUserInfo();
}

MedPay::~MedPay()
{
    delete ui;
    if (database) {
        delete database;
    }
}

void MedPay::setupDatabase()
{
    database = new Database(this);
    if (!database->connectDatabase()) {
        QMessageBox::warning(this, "数据库错误", "无法连接数据库，部分功能可能无法使用。");
    }
}

void MedPay::loadUserInfo()
{
    // 模拟用户信息，实际应用中应该从数据库或配置文件读取
    ui->nameValueLabel->setText("张三");
    ui->ageValueLabel->setText("32");
    ui->genderValueLabel->setText("男");
    ui->phoneValueLabel->setText("138****1234");
    ui->idCardValueLabel->setText("1101**********1234");
}

void MedPay::updateUserInfo()
{
    // 更新用户信息显示
    loadUserInfo();
}

void MedPay::refreshData()
{
    // 刷新数据，更新统计信息等
    if (database) {
        // 更新用户信息
        updateUserInfo();
        
        // 刷新所有子窗口的数据
        if (unpaidWindow) {
            unpaidWindow->refreshData();
        }
        if (historyWindow) {
            historyWindow->refreshData();
        }
        if (invoiceWindow) {
            invoiceWindow->refreshData();
        }
    }
}
