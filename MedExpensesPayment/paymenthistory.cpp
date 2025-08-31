#include "paymenthistory.h"
#include "ui_paymenthistory.h"
#include "database.h"
#include <QStyle>
#include <QResizeEvent>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

PaymentHistory::PaymentHistory(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaymentHistory),
    database(db),
    model(nullptr)
{
    ui->setupUi(this);

    ui->backButton->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    ui->backButton->setIconSize(QSize(16,16));

    // 如果没有传入数据库实例，则创建一个
    if (!database) {
        setupDatabase();
    }

    setupTableView();
    connectSignals();
    refreshData();
}

PaymentHistory::~PaymentHistory()
{
    delete ui;
}

void PaymentHistory::setupDatabase()
{
    database = new Database(this);
    if (!database->connectDatabase()) {
        QMessageBox::warning(this, "数据库错误", "无法连接数据库");
        return;
    }
}

void PaymentHistory::setupTableView()
{
    if (!database) return;
    
    // 设置表格模型
    model = database->getPaymentHistory();
    ui->historyTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->historyTableView->setModel(model);
    
    // 设置表格属性
    ui->historyTableView->horizontalHeader()->setStretchLastSection(true);
    ui->historyTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->historyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTableView->setAlternatingRowColors(true);
    
    // 隐藏ID列
    ui->historyTableView->hideColumn(0);
    
    // 设置列标题
    if (model) {
        model->setHeaderData(1, Qt::Horizontal, "项目名称");
        model->setHeaderData(2, Qt::Horizontal, "单价");
        model->setHeaderData(3, Qt::Horizontal, "数量");
        model->setHeaderData(4, Qt::Horizontal, "金额");
        model->setHeaderData(5, Qt::Horizontal, "收费科室");
        model->setHeaderData(6, Qt::Horizontal, "收费时间");
        model->setHeaderData(7, Qt::Horizontal, "支付方式");
        model->setHeaderData(8, Qt::Horizontal, "支付状态");
    }
}

void PaymentHistory::connectSignals()
{
    connect(ui->backButton, &QPushButton::clicked, this, &PaymentHistory::backRequested);
    connect(ui->refreshButton, &QPushButton::clicked, this, &PaymentHistory::refreshData);
    connect(ui->exportButton, &QPushButton::clicked, this, &PaymentHistory::exportData);
    connect(ui->printButton, &QPushButton::clicked, this, &PaymentHistory::printData);      // 新增：打印按钮
    
    // 连接数据库数据变化信号，自动刷新界面
    if (database) {
        connect(database, &Database::dataChanged, this, &PaymentHistory::refreshData);
    }
}

void PaymentHistory::refreshData()
{
    if (model) {
        model->select();
        updateSummary();
    }
}

void PaymentHistory::exportData()
{
    if (!model || model->rowCount() == 0) {
        QMessageBox::information(this, "提示", "没有数据可导出");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "导出缴费历史", 
                                                   "缴费历史_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".csv",
                                                   "CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    
    // 写入表头
    out << "项目名称,单价,数量,金额,收费科室,收费时间,支付方式,支付状态\n";
    
    // 写入数据
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 1; col < model->columnCount(); ++col) { // 跳过ID列
            QString data = model->data(model->index(row, col)).toString();
            // 处理CSV中的逗号和引号
            if (data.contains(",") || data.contains("\"") || data.contains("\n")) {
                data = "\"" + data.replace("\"", "\"\"") + "\"";
            }
            rowData << data;
        }
        out << rowData.join(",") << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "成功", "数据导出成功");
}

void PaymentHistory::updateSummary()
{
    if (model) {
        int count = model->rowCount();
        double total = 0.0;
        
        for (int row = 0; row < count; ++row) {
            total += model->data(model->index(row, 4)).toDouble();
        }
        
        ui->countLabel->setText(QString("已支付项目：%1项").arg(count));
        ui->totalLabel->setText(QString("已支付总额：¥%1").arg(QString::number(total, 'f', 2)));
    }
}

void PaymentHistory::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const int margin = 20;
    int x = width() - ui->backButton->width() - margin;
    int y = margin;
    ui->backButton->move(x, y);
}

void PaymentHistory::printData()
{
    if (!model || model->rowCount() == 0) {
        QMessageBox::information(this, "提示", "没有数据可打印");
        return;
    }
    
    // 生成打印内容
    QString printContent = QString(
        "医疗费用缴费历史报表\n"
        "====================\n"
        "生成时间: %1\n"
        "====================\n"
    ).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    // 添加表头
    printContent += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n")
        .arg("项目名称", 12)
        .arg("单价", 8)
        .arg("数量", 6)
        .arg("金额", 10)
        .arg("收费科室", 10)
        .arg("收费时间", 16)
        .arg("支付方式", 10)
        .arg("支付状态", 8);
    
    printContent += "====================\n";
    
    // 添加数据行
    double totalAmount = 0.0;
    for (int row = 0; row < model->rowCount(); ++row) {
        QString itemName = model->data(model->index(row, 1)).toString();
        double unitPrice = model->data(model->index(row, 2)).toDouble();
        int quantity = model->data(model->index(row, 3)).toInt();
        double amount = model->data(model->index(row, 4)).toDouble();
        QString department = model->data(model->index(row, 5)).toString();
        QString chargeTime = model->data(model->index(row, 6)).toString();
        QString paymentMethod = model->data(model->index(row, 7)).toString();
        QString paymentStatus = model->data(model->index(row, 8)).toString();
        
        totalAmount += amount;
        
        printContent += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8\n")
            .arg(itemName.leftJustified(12))
            .arg(QString::number(unitPrice, 'f', 2).leftJustified(8))
            .arg(QString::number(quantity).leftJustified(6))
            .arg(QString::number(amount, 'f', 2).leftJustified(10))
            .arg(department.leftJustified(10))
            .arg(chargeTime.leftJustified(16))
            .arg(paymentMethod.leftJustified(10))
            .arg(paymentStatus.leftJustified(8));
    }
    
    printContent += "====================\n";
    printContent += QString("总计: %1项, 总金额: ¥%2\n")
        .arg(model->rowCount())
        .arg(QString::number(totalAmount, 'f', 2));
    
    // 创建打印对话框
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QPrintDialog::Accepted) {
        QTextDocument document;
        document.setPlainText(printContent);
        document.print(&printer);
        QMessageBox::information(this, "成功", "缴费历史打印成功");
    }
}

