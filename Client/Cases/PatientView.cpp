#include "PatientView.h"
#include "ui_PatientView.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

PatientView::PatientView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PatientView),
    m_networkManager(nullptr),
    m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);

    qDebug() << "PatientView constructor called";

    // 检查UI组件
    if (!ui->virusEdit) {
        qDebug() << "Error: virusEdit not found in UI";
    }
    if (!ui->searchButton) {
        qDebug() << "Error: searchButton not found in UI";
    }
    if (!ui->tableView) {
        qDebug() << "Error: tableView not found in UI";
    }
    if (!ui->statusLabel) {
        qDebug() << "Error: statusLabel not found in UI";
    }

    setupTable();

    // 连接搜索按钮
    bool connected = connect(ui->searchButton, &QPushButton::clicked, this, &PatientView::onSearchButtonClicked);
    qDebug() << "Search button connection:" << connected;
    connect(&CaseNetworkManager::instance(), &CaseNetworkManager::dataReceived, this, &PatientView::handleDataReceived);
}

PatientView::~PatientView()
{
    delete ui;
}


void PatientView::setupTable()
{
    m_model->setHorizontalHeaderLabels({"患者姓名", "病毒类型", "诊断日期", "主治医生", "科室"});
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void PatientView::onSearchButtonClicked()
{
    qDebug() << "Search button clicked in PatientView";

    QString virus = ui->virusEdit->text().trimmed();

    if (virus.isEmpty()) {
        ui->statusLabel->setText("请输入病毒名称");
        return;
    }

    QJsonObject request;
    request.insert("type", "patient_query");
    request.insert("virus", virus);

    m_networkManager->sendRequest(request);
    ui->statusLabel->setText("正在查询...");
}

void PatientView::handleDataReceived(const QJsonObject &data)
{
    QString type = data.value("type").toString();

    if (type != "patient_query_response") {
        return; // 不是我们要处理的响应类型
    }

    QString status = data.value("status").toString();
    if (status == "error") {
        ui->statusLabel->setText("查询错误: " + data.value("message").toString());
        return;
    }

    QJsonArray cases = data.value("data").toArray();
    m_model->removeRows(0, m_model->rowCount());

    if (cases.isEmpty()) {
        ui->statusLabel->setText("未找到相关病例记录");
        return;
    }

    for (const QJsonValue &caseValue : cases) {
        QJsonObject caseObj = caseValue.toObject();

        QList<QStandardItem*> items;
        items << new QStandardItem(caseObj.value("patient_name").toString());
        items << new QStandardItem(caseObj.value("virus_type").toString());
        items << new QStandardItem(caseObj.value("diagnosis_date").toString());
        items << new QStandardItem(caseObj.value("doctor_name").toString());
        items << new QStandardItem(caseObj.value("department").toString());

        m_model->appendRow(items);
    }

    ui->statusLabel->setText(QString("找到 %1 条记录").arg(cases.size()));
}
