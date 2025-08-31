#include "DataViewer.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>


DataViewer::DataViewer(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent)
    , m_dbManager(dbManager)
{
    setupUI();
    loadData();
}

void DataViewer::setupUI()
{
    setWindowTitle("患者数据表");
    setMinimumSize(1520, 600);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 搜索区域
    QHBoxLayout *searchLayout = new QHBoxLayout();
    
    QLabel *searchLabel = new QLabel("搜索:");
    m_searchFieldCombo = new QComboBox();
    m_searchFieldCombo->addItems({"姓名", "手机号"});
    
    m_searchLineEdit = new QLineEdit();
    m_searchLineEdit->setPlaceholderText("输入搜索关键词");
    
    m_searchButton = new QPushButton("搜索");
    m_refreshButton = new QPushButton("刷新");
    m_exportButton = new QPushButton("导出数据");
    m_clearButton = new QPushButton("清空搜索");
    
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchFieldCombo);
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addWidget(m_refreshButton);
    searchLayout->addWidget(m_exportButton);
    searchLayout->addWidget(m_clearButton);
    searchLayout->addStretch();
    
    mainLayout->addLayout(searchLayout);
    
    // 表格区域
    m_tableWidget = new QTableWidget();
    mainLayout->addWidget(m_tableWidget);
    
    setupTable();
    
    // 连接信号槽
    connect(m_searchButton, &QPushButton::clicked, this, &DataViewer::searchData);
    connect(m_refreshButton, &QPushButton::clicked, this, &DataViewer::refreshData);
    connect(m_exportButton, &QPushButton::clicked, this, &DataViewer::exportData);
    connect(m_clearButton, &QPushButton::clicked, this, &DataViewer::clearSearch);
}

void DataViewer::setupTable()
{
    QStringList headers = {
        "ID", "姓名", "年龄", "性别", "身高(cm)", "体重(kg)", 
        "手机号", "BMI", "BMI状态", "生活方式", "健康评分", "提交时间"
    };
    
    m_tableWidget->setColumnCount(headers.size());
    m_tableWidget->setHorizontalHeaderLabels(headers);
    
    // 设置表格属性 - 完全只读模式
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // 自动调整列宽
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
}

void DataViewer::loadData()
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        QMessageBox::warning(this, "错误", "数据库未连接");
        return;
    }
    
    QList<QMap<QString, QVariant>> patientList = m_dbManager->getPatientData();
    
    m_tableWidget->setRowCount(patientList.size());
    
    for (int row = 0; row < patientList.size(); ++row) {
        const QMap<QString, QVariant> &patient = patientList[row];
        
        m_tableWidget->setItem(row, 0, new QTableWidgetItem(patient["id"].toString()));
        m_tableWidget->setItem(row, 1, new QTableWidgetItem(patient["name"].toString()));
        m_tableWidget->setItem(row, 2, new QTableWidgetItem(patient["age"].toString()));
        m_tableWidget->setItem(row, 3, new QTableWidgetItem(patient["gender"].toString()));
        m_tableWidget->setItem(row, 4, new QTableWidgetItem(patient["height"].toString()));
        m_tableWidget->setItem(row, 5, new QTableWidgetItem(patient["weight"].toString()));
        m_tableWidget->setItem(row, 6, new QTableWidgetItem(patient["phone"].toString()));
        // BMI保留一位小数
        double bmi = patient["bmi"].toDouble();
        QString bmiText = QString::number(bmi, 'f', 1);
        m_tableWidget->setItem(row, 7, new QTableWidgetItem(bmiText));
        m_tableWidget->setItem(row, 8, new QTableWidgetItem(patient["bmi_status"].toString()));
        m_tableWidget->setItem(row, 9, new QTableWidgetItem(patient["lifestyle_choices"].toString()));
        m_tableWidget->setItem(row, 10, new QTableWidgetItem(patient["health_score"].toString()));
        m_tableWidget->setItem(row, 11, new QTableWidgetItem(patient["submission_time"].toString()));
    }
    
    m_tableWidget->resizeColumnsToContents();
}

void DataViewer::searchData()
{
    QString searchText = m_searchLineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        loadData();
        return;
    }
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        QMessageBox::warning(this, "错误", "数据库未连接");
        return;
    }
    
    QList<QMap<QString, QVariant>> patientList;
    QString searchField = m_searchFieldCombo->currentText();
    
    if (searchField == "姓名") {
        QMap<QString, QVariant> patient = m_dbManager->getPatientByName(searchText);
        if (!patient.isEmpty()) {
            patientList.append(patient);
        }
    } else if (searchField == "手机号") {
        QMap<QString, QVariant> patient = m_dbManager->getPatientByPhone(searchText);
        if (!patient.isEmpty()) {
            patientList.append(patient);
        }
    }
    
    // 显示搜索结果
    m_tableWidget->setRowCount(patientList.size());
    
    for (int row = 0; row < patientList.size(); ++row) {
        const QMap<QString, QVariant> &patient = patientList[row];
        
        m_tableWidget->setItem(row, 0, new QTableWidgetItem(patient["id"].toString()));
        m_tableWidget->setItem(row, 1, new QTableWidgetItem(patient["name"].toString()));
        m_tableWidget->setItem(row, 2, new QTableWidgetItem(patient["age"].toString()));
        m_tableWidget->setItem(row, 3, new QTableWidgetItem(patient["gender"].toString()));
        m_tableWidget->setItem(row, 4, new QTableWidgetItem(patient["height"].toString()));
        m_tableWidget->setItem(row, 5, new QTableWidgetItem(patient["weight"].toString()));
        m_tableWidget->setItem(row, 6, new QTableWidgetItem(patient["phone"].toString()));
        // BMI保留一位小数
        double bmi = patient["bmi"].toDouble();
        QString bmiText = QString::number(bmi, 'f', 1);
        m_tableWidget->setItem(row, 7, new QTableWidgetItem(bmiText));
        m_tableWidget->setItem(row, 8, new QTableWidgetItem(patient["bmi_status"].toString()));
        m_tableWidget->setItem(row, 9, new QTableWidgetItem(patient["lifestyle_choices"].toString()));
        m_tableWidget->setItem(row, 10, new QTableWidgetItem(patient["health_score"].toString()));
        m_tableWidget->setItem(row, 11, new QTableWidgetItem(patient["submission_time"].toString()));
    }
    
    m_tableWidget->resizeColumnsToContents();
}

void DataViewer::exportData()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/患者数据.csv", 
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
    QStringList headers = {
        "ID", "姓名", "年龄", "性别", "身高(cm)", "体重(kg)", 
        "手机号", "BMI", "BMI状态", "生活方式选择", "健康评分", "提交时间"
    };
    
    out << headers.join(",") << "\n";
    
    // 写入数据
    for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = m_tableWidget->item(row, col);
            QString text = item ? item->text() : "";
            // 处理CSV中的逗号
            if (text.contains(",")) {
                text = "\"" + text + "\"";
            }
            rowData << text;
        }
        out << rowData.join(",") << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "成功", "数据导出成功");
}

void DataViewer::clearSearch()
{
    m_searchLineEdit->clear();
    loadData();
}

void DataViewer::refreshData()
{
    loadData();
}
