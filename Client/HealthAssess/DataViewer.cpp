#include "DataViewer.h"
#include <QJsonArray>

DataViewer::DataViewer(NetClient *client, QWidget *parent)
    : QWidget(parent)
    , m_client(client)
{
    setupUI();
    loadData();
    connect(client, &NetClient::allPatientsReceived, this, &DataViewer::handleAllPatients);
}

void DataViewer::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    auto *searchLayout = new QHBoxLayout();

    m_searchFieldCombo = new QComboBox(this);
    m_searchFieldCombo->addItems(QStringList() << "姓名" << "手机号");

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("输入检索词…");

    m_searchButton = new QPushButton("搜索", this);
    m_refreshButton = new QPushButton("刷新", this);
    m_exportButton = new QPushButton("导出 CSV", this);
    m_clearButton = new QPushButton("清空检索", this);

    searchLayout->addWidget(new QLabel("字段：", this));
    searchLayout->addWidget(m_searchFieldCombo, 0);
    searchLayout->addWidget(m_searchLineEdit, 1);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addWidget(m_refreshButton);
    searchLayout->addWidget(m_clearButton);
    searchLayout->addStretch();
    searchLayout->addWidget(m_exportButton);

    m_tableWidget = new QTableWidget(this);
    setupTable();

    layout->addLayout(searchLayout);
    layout->addWidget(m_tableWidget);
    setLayout(layout);

    connect(m_searchButton,  &QPushButton::clicked, this, &DataViewer::searchData);
    connect(m_refreshButton, &QPushButton::clicked, this, &DataViewer::loadData);
    connect(m_clearButton,   &QPushButton::clicked, this, &DataViewer::clearSearch);
    connect(m_exportButton,  &QPushButton::clicked, this, &DataViewer::exportData);
}

void DataViewer::setupTable()
{
    m_tableWidget->setColumnCount(12);
    QStringList headers = {
        "ID", "姓名", "年龄", "性别", "身高(cm)", "体重(kg)", "手机号",
        "BMI", "BMI状态", "生活方式", "健康评分", "提交时间"
    };
    m_tableWidget->setHorizontalHeaderLabels(headers);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DataViewer::loadData() {
    NetClient::instance().getAll();
}

void DataViewer::searchData()
{
    const QString searchText = m_searchLineEdit->text().trimmed();
    if (searchText.isEmpty()) { loadData(); return; }
    if (!m_client) return;

    const QString field = m_searchFieldCombo->currentText();
    auto fillOne = [this](const QJsonObject& obj){
        if (obj.isEmpty()) { m_tableWidget->setRowCount(0); return; }
        m_tableWidget->setRowCount(1);
        auto at = [&](const char* k){ return obj.value(k).toVariant().toString(); };
        m_tableWidget->setItem(0,0,new QTableWidgetItem(at("id")));
        m_tableWidget->setItem(0,1,new QTableWidgetItem(at("name")));
        m_tableWidget->setItem(0,2,new QTableWidgetItem(at("age")));
        m_tableWidget->setItem(0,3,new QTableWidgetItem(at("gender")));
        m_tableWidget->setItem(0,4,new QTableWidgetItem(at("height")));
        m_tableWidget->setItem(0,5,new QTableWidgetItem(at("weight")));
        m_tableWidget->setItem(0,6,new QTableWidgetItem(at("phone")));
        QString bmiText = QString::number(obj.value("bmi").toDouble(), 'f', 1);
        m_tableWidget->setItem(0,7,new QTableWidgetItem(bmiText));
        m_tableWidget->setItem(0,8,new QTableWidgetItem(at("bmi_status")));
        m_tableWidget->setItem(0,9,new QTableWidgetItem(at("lifestyle_choices")));
        m_tableWidget->setItem(0,10,new QTableWidgetItem(at("health_score")));
        m_tableWidget->setItem(0,11,new QTableWidgetItem(at("submission_time")));
        m_tableWidget->resizeColumnsToContents();
    };

        NetClient& client = NetClient::instance();

        if (field == "姓名") {
            // Connect once to the signal, call fillOne when data arrives
            connect(&client, &NetClient::patientByNameReceived, this, [fillOne](const QJsonObject &obj){
                fillOne(obj);
            });

            client.getByName(searchText);
        } else {
            connect(&client, &NetClient::patientByPhoneReceived, this, [fillOne](const QJsonObject &obj){
                fillOne(obj);
            });

            client.getByPhone(searchText);
        }
}

void DataViewer::exportData()
{
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this, "导出 CSV", defaultDir + "/health_data.csv",
                                                    "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建导出文件");
        return;
    }
    QTextStream out(&file);
    // 表头
    QStringList headers;
    for (int c=0; c<m_tableWidget->columnCount(); ++c)
        headers << m_tableWidget->horizontalHeaderItem(c)->text();
    out << headers.join(",") << "\n";

    // 内容
    for (int r=0; r<m_tableWidget->rowCount(); ++r) {
        QStringList cols;
        for (int c=0; c<m_tableWidget->columnCount(); ++c) {
            auto *item = m_tableWidget->item(r, c);
            cols << (item ? item->text() : "");
        }
        out << cols.join(",") << "\n";
    }
    file.close();
    QMessageBox::information(this, "导出完成", "CSV 导出成功！");
}

void DataViewer::clearSearch()
{
    m_searchLineEdit->clear();
    loadData();
}

void DataViewer::handleAllPatients(const QJsonArray &arr) {
    if (arr.isEmpty()) {
        QMessageBox::warning(this, "错误", "后端查询失败或无数据");
        return;
    }

    m_tableWidget->setRowCount(arr.size());
    for (int row = 0; row < arr.size(); ++row) {
        auto obj = arr[row].toObject();
        auto at = [&](const char* k){ return obj.value(k).toVariant().toString(); };
        m_tableWidget->setItem(row, 0, new QTableWidgetItem(at("id")));
        m_tableWidget->setItem(row, 1, new QTableWidgetItem(at("name")));
        m_tableWidget->setItem(row, 2, new QTableWidgetItem(at("age")));
        m_tableWidget->setItem(row, 3, new QTableWidgetItem(at("gender")));
        m_tableWidget->setItem(row, 4, new QTableWidgetItem(at("height")));
        m_tableWidget->setItem(row, 5, new QTableWidgetItem(at("weight")));
        m_tableWidget->setItem(row, 6, new QTableWidgetItem(at("phone")));
        QString bmiText = QString::number(obj.value("bmi").toDouble(), 'f', 1);
        m_tableWidget->setItem(row, 7, new QTableWidgetItem(bmiText));
        m_tableWidget->setItem(row, 8, new QTableWidgetItem(at("bmi_status")));
        m_tableWidget->setItem(row, 9, new QTableWidgetItem(at("lifestyle_choices")));
        m_tableWidget->setItem(row,10, new QTableWidgetItem(at("health_score")));
        m_tableWidget->setItem(row,11, new QTableWidgetItem(at("submission_time")));
    }
    m_tableWidget->resizeColumnsToContents();
}
