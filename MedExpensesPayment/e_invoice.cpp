#include "e_invoice.h"
#include "ui_e_invoice.h"
#include "database.h"
#include <QStyle>
#include <QResizeEvent>
#include <QMessageBox>
#include <QHeaderView>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>

E_Invoice::E_Invoice(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::E_Invoice),
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
    setupFilters();        // 新增：设置筛选控件
    refreshData();
}

E_Invoice::~E_Invoice()
{
    delete ui;
}

void E_Invoice::setupDatabase()
{
    database = new Database(this);
    if (!database->connectDatabase()) {
        QMessageBox::warning(this, "数据库错误", "无法连接数据库");
        return;
    }
}

void E_Invoice::setupTableView()
{
    if (!database) return;
    
    // 设置表格模型
    model = database->getInvoiceList();
    ui->invoiceTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->invoiceTableView->setModel(model);
    
    // 设置表格属性
    ui->invoiceTableView->horizontalHeader()->setStretchLastSection(true);
    ui->invoiceTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->invoiceTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->invoiceTableView->setAlternatingRowColors(true);
    
    // 隐藏ID列
    ui->invoiceTableView->hideColumn(0);
    
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

void E_Invoice::connectSignals()
{
    connect(ui->backButton, &QPushButton::clicked, this, &E_Invoice::backRequested);
    connect(ui->refreshButton, &QPushButton::clicked, this, &E_Invoice::refreshData);
    connect(ui->generateInvoiceButton, &QPushButton::clicked, this, &E_Invoice::generateInvoice);
    connect(ui->printButton, &QPushButton::clicked, this, &E_Invoice::printInvoice);
    connect(ui->downloadButton, &QPushButton::clicked, this, &E_Invoice::downloadData);
    connect(ui->filterButton, &QPushButton::clicked, this, &E_Invoice::applyFilters);
    
    // 连接筛选控件的信号
    connect(ui->statusFilterCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
            this, &E_Invoice::applyFilters);
    connect(ui->departmentFilterCombo, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
            this, &E_Invoice::applyFilters);
    
    // 连接数据库数据变化信号，自动刷新界面
    if (database) {
        connect(database, &Database::dataChanged, this, &E_Invoice::refreshData);
    }
}

void E_Invoice::refreshData()
{
    if (model) {
        model->select();
        updateSummary();
    }
}

void E_Invoice::generateInvoice()
{
    QItemSelectionModel *selection = ui->invoiceTableView->selectionModel();
    if (!selection->hasSelection()) {
        QMessageBox::information(this, "提示", "请先选择要生成票据的项目");
        return;
    }

    QModelIndexList selectedRows = selection->selectedRows();
    if (selectedRows.size() > 1) {
        QMessageBox::information(this, "提示", "一次只能选择一个项目生成票据");
        return;
    }

    int row = selectedRows.first().row();
    QString itemName = model->data(model->index(row, 1)).toString();
    double amount = model->data(model->index(row, 4)).toDouble();
    QString department = model->data(model->index(row, 5)).toString();
    QString paymentMethod = model->data(model->index(row, 7)).toString();
    QString paymentStatus = model->data(model->index(row, 8)).toString();

    // 生成票据内容
    QString invoiceContent = QString(
        "医疗费用票据\n"
        "================\n"
        "项目名称: %1\n"
        "金额: ¥%2\n"
        "收费科室: %3\n"
        "支付方式: %4\n"
        "支付状态: %5\n"
        "生成时间: %6\n"
        "================\n"
        "票据编号: INV%7"
    ).arg(itemName)
     .arg(QString::number(amount, 'f', 2))
     .arg(department)
     .arg(paymentMethod)
     .arg(paymentStatus)
     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
     .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));

    ui->invoiceTextEdit->setPlainText(invoiceContent);
    QMessageBox::information(this, "成功", "票据生成成功");
}

void E_Invoice::printInvoice()
{
    if (ui->invoiceTextEdit->toPlainText().isEmpty()) {
        QMessageBox::information(this, "提示", "请先生成票据");
        return;
    }

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QPrintDialog::Accepted) {
        QTextDocument document;
        document.setPlainText(ui->invoiceTextEdit->toPlainText());
        document.print(&printer);
        QMessageBox::information(this, "成功", "票据打印成功");
    }
}

void E_Invoice::updateSummary()
{
    if (model) {
        int totalCount = model->rowCount();
        int paidCount = 0;
        int unpaidCount = 0;
        double totalAmount = 0.0;
        double paidAmount = 0.0;
        double unpaidAmount = 0.0;
        
        for (int row = 0; row < totalCount; ++row) {
            double amount = model->data(model->index(row, 4)).toDouble();
            QString status = model->data(model->index(row, 8)).toString();
            
            totalAmount += amount;
            if (status == "已支付") {
                paidCount++;
                paidAmount += amount;
            } else {
                unpaidCount++;
                unpaidAmount += amount;
            }
        }
        
        // 只更新存在的标签，移除不存在的标签引用
        // ui->totalCountLabel->setText(QString("总项目数：%1项").arg(totalCount));
        // ui->paidCountLabel->setText(QString("已支付：%1项").arg(paidCount));
        // ui->unpaidCountLabel->setText(QString("待支付：%1项").arg(unpaidCount));
        // ui->totalAmountLabel->setText(QString("总金额：¥%1").arg(QString::number(totalAmount, 'f', 2)));
        // ui->paidAmountLabel->setText(QString("已支付：¥%1").arg(QString::number(paidAmount, 'f', 2)));
        // ui->unpaidAmountLabel->setText(QString("待支付：¥%1").arg(QString::number(unpaidAmount, 'f', 2)));
        
        // 输出到调试信息
        qDebug() << "总项目数:" << totalCount << "已支付:" << paidCount << "待支付:" << unpaidCount;
        qDebug() << "总金额:" << totalAmount << "已支付:" << paidAmount << "待支付:" << unpaidAmount;
    }
}

void E_Invoice::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    const int margin = 20;
    int x = width() - ui->backButton->width() - margin;
    int y = margin;
    ui->backButton->move(x, y);
}

void E_Invoice::setupFilters()
{
    // 初始化筛选控件
    ui->statusFilterCombo->setCurrentText("全部状态");
    ui->departmentFilterCombo->setCurrentText("全部科室");
}

void E_Invoice::downloadData()
{
    if (!model) {
        QMessageBox::warning(this, "错误", "没有数据可下载");
        return;
    }

    // 检查是否已生成票据
    QString currentInvoice = ui->invoiceTextEdit->toPlainText();
    bool hasInvoice = !currentInvoice.isEmpty();
    
    QString fileName;
    if (hasInvoice) {
        // 如果已生成票据，只下载该票据
        fileName = QFileDialog::getSaveFileName(this, "保存票据", 
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/医疗费用票据.txt",
            "文本文件 (*.txt);;所有文件 (*)");
    } else {
        // 如果没有生成票据，下载全部数据
        fileName = QFileDialog::getSaveFileName(this, "保存数据", 
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/医疗费用数据.csv",
            "CSV文件 (*.csv);;文本文件 (*.txt)");
    }
    
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件：" + file.errorString());
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8"); // 设置编码为UTF-8，支持中文

    if (hasInvoice) {
        // 下载生成的票据
        out << currentInvoice;
        file.close();
        QMessageBox::information(this, "成功", "票据已成功下载到：" + fileName);
    } else {
        // 下载全部数据为CSV格式
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
        QMessageBox::information(this, "成功", "全部数据已成功下载到：" + fileName);
    }
}

void E_Invoice::applyFilters()
{
    if (!model) return;

    QString statusFilter = ui->statusFilterCombo->currentText();
    QString departmentFilter = ui->departmentFilterCombo->currentText();

    QString filterString;
    
    // 构建筛选条件
    if (statusFilter != "全部状态") {
        filterString += QString("payment_status = '%1'").arg(statusFilter);
    }
    
    if (departmentFilter != "全部科室") {
        if (!filterString.isEmpty()) {
            filterString += " AND ";
        }
        filterString += QString("department = '%1'").arg(departmentFilter);
    }

    // 应用筛选
    if (filterString.isEmpty()) {
        model->setFilter("");
    } else {
        model->setFilter(filterString);
    }
    
    model->select();
    updateSummary();
}

void E_Invoice::resetFilters()
{
    ui->statusFilterCombo->setCurrentText("全部状态");
    ui->departmentFilterCombo->setCurrentText("全部科室");
    applyFilters();
}

