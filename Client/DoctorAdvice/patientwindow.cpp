#include "patientwindow.h"
#include "orderdetaildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

PatientWindow::PatientWindow(Client *client, const QString &patientId, QWidget *parent)
    : QWidget(parent), client_(client), patientId_(patientId)
{
    setWindowTitle(QString("患者界面 - %1").arg(patientId_));
    QVBoxLayout *lay = new QVBoxLayout(this);

    QHBoxLayout *row = new QHBoxLayout;
    row->addWidget(new QLabel("我的患者ID:"));
    editPatientId_ = new QLineEdit(this);
    editPatientId_->setText(patientId_);
    editPatientId_->setReadOnly(true);
    row->addWidget(editPatientId_);

    btnRefresh_ = new QPushButton("刷新我的医嘱", this);
    row->addWidget(btnRefresh_);
    lay->addLayout(row);

    table_ = new QTableWidget(0, 6, this);
    QStringList headers;
    headers << "ID" << "患者ID" << "医生ID" << "科室" << "摘要" << "时间";
    table_->setHorizontalHeaderLabels(headers);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    lay->addWidget(table_);

    connect(btnRefresh_, SIGNAL(clicked()), this, SLOT(refreshOrders()));
    connect(table_, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onCellDoubleClicked(int,int)));
    connect(client_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));

    refreshOrders();
}

void PatientWindow::refreshOrders() {
    QJsonObject o;
    o.insert("type", "list_orders");
    o.insert("patient_id", patientId_);
    o.insert("keyword", "");
    o.insert("dept", "全部");
    o.insert("date_from", "");
    o.insert("date_to", "");
    o.insert("page", 1);
    o.insert("page_size", 50);
    client_->sendJson(o);
}

void PatientWindow::onJson(const QJsonObject &obj) {
    const QString type = obj.value("type").toString();
    if (type == "orders" && obj.value("patient_id").toString() == patientId_) {
        QJsonArray items = obj.value("items").toArray();
        items_.clear();
        table_->setRowCount(items.size());
        int row = 0;
        for (int i = 0; i < items.size(); ++i) {
            QJsonObject o = items.at(i).toObject();
            items_.append(o);
            table_->setItem(row,0,new QTableWidgetItem(QString::number(o.value("id").toInt())));
            table_->setItem(row,1,new QTableWidgetItem(o.value("patient_id").toString()));
            table_->setItem(row,2,new QTableWidgetItem(o.value("doctor_id").toString()));
            table_->setItem(row,3,new QTableWidgetItem(o.value("dept").toString()));
            QString content = o.value("content").toString();
            QString summary = content.left(24);
            if (content.length() > 24) summary += "…";
            table_->setItem(row,4,new QTableWidgetItem(summary));
            table_->setItem(row,5,new QTableWidgetItem(o.value("created_at").toString()));
            ++row;
        }
    }
}

void PatientWindow::onCellDoubleClicked(int row, int) {
    if (row < 0 || row >= items_.size()) return;
    QJsonObject o = items_.at(row);
    OrderDetailDialog dlg(client_, o, false, this); // 只读
    dlg.exec();
}
