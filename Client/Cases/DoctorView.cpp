#include "DoctorView.h"
#include "ui_DoctorView.h"
#include "CaseEditDialog.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QToolBar>
#include <QPushButton>
#include <QMessageBox>
#include <QModelIndex>
#include <QVBoxLayout>

DoctorView::DoctorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoctorView),
    m_networkManager(nullptr),
    m_model(new QStandardItemModel(this)),
    m_editButton(nullptr)
{
    ui->setupUi(this);

    setupTable();
    setupToolbar();

    connect(ui->searchButton, &QPushButton::clicked, this, &DoctorView::onSearchButtonClicked);
    connect(ui->tableView, &QTableView::doubleClicked, this, &DoctorView::onTableDoubleClicked);
    connect(&CaseNetworkManager::instance(), &CaseNetworkManager::dataReceived, this, &DoctorView::handleDataReceived);
}

DoctorView::~DoctorView()
{
    delete ui;
}


void DoctorView::setupTable()
{
    m_model->setHorizontalHeaderLabels({"ID", "患者姓名", "患者ID", "病毒类型", "诊断日期", "症状", "治疗方案", "医生ID", "医生姓名", "科室"});
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DoctorView::setupToolbar()
{
    QToolBar *toolbar = new QToolBar(this);
    m_editButton = new QPushButton("编辑病例", this);
    toolbar->addWidget(m_editButton);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        layout->insertWidget(0, toolbar);
    }

    connect(m_editButton, &QPushButton::clicked, this, &DoctorView::onEditButtonClicked);
}

void DoctorView::onSearchButtonClicked()
{
    QString virus = ui->virusEdit->text().trimmed();

    if (virus.isEmpty()) {
        ui->statusLabel->setText("请输入病毒名称");
        return;
    }

    QJsonObject request;
    request.insert("type", "doctor_query");
    request.insert("virus", virus);

    m_networkManager->sendRequest(request);
    ui->statusLabel->setText("正在查询...");
}

void DoctorView::onEditButtonClicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要编辑的病例");
        return;
    }

    int row = selectedIndexes.first().row();
    editCase(row);
}

void DoctorView::onTableDoubleClicked(const QModelIndex &index)
{
    editCase(index.row());
}

void DoctorView::editCase(int row)
{
    // 获取选中病例的数据
    QJsonObject caseData;
    caseData.insert("id", m_model->item(row, 0)->text().toInt());
    caseData.insert("patient_name", m_model->item(row, 1)->text());
    caseData.insert("patient_id", m_model->item(row, 2)->text());
    caseData.insert("virus_type", m_model->item(row, 3)->text());
    caseData.insert("diagnosis_date", m_model->item(row, 4)->text());
    caseData.insert("symptoms", m_model->item(row, 5)->text());
    caseData.insert("treatment", m_model->item(row, 6)->text());
    caseData.insert("doctor_id", m_model->item(row, 7)->text());
    caseData.insert("doctor_name", m_model->item(row, 8)->text());
    caseData.insert("department", m_model->item(row, 9)->text());

    // 打开编辑对话框
    CaseEditDialog dialog(this);
    dialog.setCaseData(caseData);

    if (dialog.exec() == QDialog::Accepted) {
        // 发送更新请求
        QJsonObject request;
        request.insert("type", "update_case");
        request.insert("case_data", dialog.getCaseData());

        m_networkManager->sendRequest(request);

        // 连接更新响应信号
        connect(m_networkManager, &CaseNetworkManager::dataReceived,
                this, &DoctorView::handleUpdateResponse, Qt::UniqueConnection);
    }
}

void DoctorView::handleUpdateResponse(const QJsonObject &data)
{
    QString type = data.value("type").toString();

    if (type != "update_case_response") {
        return;
    }

    QString status = data.value("status").toString();
    if (status == "success") {
        QMessageBox::information(this, "成功", "病例更新成功");
        // 刷新表格
        onSearchButtonClicked();
    } else {
        QMessageBox::warning(this, "错误", "病例更新失败: " + data.value("message").toString());
    }

    // 断开连接
    disconnect(m_networkManager, &CaseNetworkManager::dataReceived,
               this, &DoctorView::handleUpdateResponse);
}

void DoctorView::handleDataReceived(const QJsonObject &data)
{
    QString type = data.value("type").toString();

    if (type != "doctor_query_response") {
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
        items << new QStandardItem(QString::number(caseObj.value("id").toInt()));
        items << new QStandardItem(caseObj.value("patient_name").toString());
        items << new QStandardItem(caseObj.value("patient_id").toString());
        items << new QStandardItem(caseObj.value("virus_type").toString());
        items << new QStandardItem(caseObj.value("diagnosis_date").toString());
        items << new QStandardItem(caseObj.value("symptoms").toString());
        items << new QStandardItem(caseObj.value("treatment").toString());
        items << new QStandardItem(caseObj.value("doctor_id").toString());
        items << new QStandardItem(caseObj.value("doctor_name").toString());
        items << new QStandardItem(caseObj.value("department").toString());

        m_model->appendRow(items);
    }

    ui->statusLabel->setText(QString("找到 %1 条记录").arg(cases.size()));
}
