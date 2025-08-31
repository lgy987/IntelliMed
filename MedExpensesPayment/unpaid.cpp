#include "unpaid.h"
#include "ui_unpaid.h"
#include "database.h"
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QHeaderView>
#include <QDateTime>

Unpaid::Unpaid(Database *db, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Unpaid)
    , database(db)
    , model(nullptr)
{
    ui->setupUi(this);

    // 如果没有传入数据库实例，则创建一个
    if (!database) {
        database = new Database(this);
        if (!database->connectDatabase()) {
            QMessageBox::warning(this, "数据库错误", "无法连接数据库");
            return;
        }
    }

    setupTableView();
    connectSignals();
    refreshData();
}

Unpaid::~Unpaid()
{
    delete ui;
}

void Unpaid::setupTableView()
{
    // 设置表格模型
    model = database->getUnpaidExpenses();
    ui->expensesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->expensesTableView->setModel(model);
    
    // 设置表格属性
    ui->expensesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->expensesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->expensesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->expensesTableView->setAlternatingRowColors(true);
    
    // 隐藏ID列
    ui->expensesTableView->hideColumn(0);
    
    // 设置列标题
    model->setHeaderData(1, Qt::Horizontal, "项目名称");
    model->setHeaderData(2, Qt::Horizontal, "单价");
    model->setHeaderData(3, Qt::Horizontal, "数量");
    model->setHeaderData(4, Qt::Horizontal, "金额");
    model->setHeaderData(5, Qt::Horizontal, "收费科室");
    model->setHeaderData(6, Qt::Horizontal, "收费时间");
    model->setHeaderData(7, Qt::Horizontal, "支付方式");
    model->setHeaderData(8, Qt::Horizontal, "支付状态");
}

void Unpaid::connectSignals()
{
    connect(ui->backButton, &QPushButton::clicked, this, &Unpaid::backRequested);
    connect(ui->refreshButton, &QPushButton::clicked, this, &Unpaid::refreshData);
    connect(ui->paySelectedButton, &QPushButton::clicked, this, &Unpaid::paySelected);
    connect(ui->payAllButton, &QPushButton::clicked, this, &Unpaid::payAll);
    connect(ui->insertButton, &QPushButton::clicked, this, &Unpaid::insertData); // 连接新增按钮信号
    
    // 连接数据库数据变化信号，自动刷新界面
    if (database) {
        connect(database, &Database::dataChanged, this, &Unpaid::refreshData);
    }
}

// 新增插入数据函数
void Unpaid::insertData()
{
    // 创建一条测试数据
    QString itemName = "测试项目" + QString::number(QDateTime::currentDateTime().toSecsSinceEpoch() % 1000);
    double unitPrice = 50.0 + (QDateTime::currentDateTime().toSecsSinceEpoch() % 100);
    int quantity = 1 + (QDateTime::currentDateTime().toSecsSinceEpoch() % 5);
    double amount = unitPrice * quantity;
    QString department = "测试科室";
    QString paymentMethod = "自费支付";
    
    // 插入数据
    if (database->insertMedicalExpense(itemName, unitPrice, quantity, amount, 
                                      department, paymentMethod, "待支付")) {
        QMessageBox::information(this, "成功", "成功插入测试数据");
        refreshData(); // 刷新显示
    } else {
        QMessageBox::warning(this, "失败", "插入测试数据失败");
    }
}

void Unpaid::refreshData()
{
    if (model) {
        model->select();
        updateSummary();
    }
}

void Unpaid::paySelected()
{
    QItemSelectionModel *selection = ui->expensesTableView->selectionModel();
    if (!selection->hasSelection()) {
        QMessageBox::information(this, "提示", "请先选择要支付的项目");
        return;
    }

    QModelIndexList selectedRows = selection->selectedRows();
    int count = selectedRows.size();
    
    if (QMessageBox::question(this, "确认支付", 
                             QString("确定要支付选中的 %1 个项目吗？").arg(count)) == QMessageBox::Yes) {
        
        int successCount = 0;
        for (const QModelIndex &index : selectedRows) {
            int row = index.row();
            int id = model->data(model->index(row, 0)).toInt();
            
            if (database->updatePaymentStatus(id, "已支付")) {
                successCount++;
            }
        }
        
        QMessageBox::information(this, "支付结果", 
                               QString("成功支付 %1 个项目").arg(successCount));
        
        refreshData();
    }
}

void Unpaid::payAll()
{
    int totalCount = model->rowCount();
    if (totalCount == 0) {
        QMessageBox::information(this, "提示", "没有待支付的项目");
        return;
    }
    
    if (QMessageBox::question(this, "确认支付", 
                             QString("确定要支付所有 %1 个待支付项目吗？").arg(totalCount)) == QMessageBox::Yes) {
        
        int successCount = 0;
        for (int row = 0; row < totalCount; ++row) {
            int id = model->data(model->index(row, 0)).toInt();
            
            if (database->updatePaymentStatus(id, "已支付")) {
                successCount++;
            }
        }
        
        QMessageBox::information(this, "支付结果", 
                               QString("成功支付 %1 个项目").arg(successCount));
        
        // 支付完成后刷新数据
        refreshData();
        
        // 手动发射数据变化信号，确保其他界面也能刷新
        if (database) {
            emit database->dataChanged();
        }
    }
}

void Unpaid::updateSummary()
{
    if (model) {
        int count = model->rowCount();
        double total = 0.0;
        
        for (int row = 0; row < count; ++row) {
            total += model->data(model->index(row, 4)).toDouble();
        }
        
        ui->countLabel->setText(QString("待支付项目：%1项").arg(count));
        ui->totalLabel->setText(QString("待支付总额：¥%1").arg(QString::number(total, 'f', 2)));
        
        // 根据是否有数据启用/禁用按钮
        ui->payAllButton->setEnabled(count > 0);
        ui->paySelectedButton->setEnabled(count > 0);
    }
}
