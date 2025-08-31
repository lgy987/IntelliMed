#include "doctorwindow.h"
#include "orderdetaildialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QJsonObject>
#include <QJsonArray>

DoctorWindow::DoctorWindow(Client *client, const QString &doctorId,
                           const QString &doctorName, QWidget *parent)
    : QWidget(parent), client_(client), doctorId_(doctorId), doctorName_(doctorName),
      currentPage_(1), total_(0), pageSize_(10)
{
    setWindowTitle(QString("医生界面 - %1(%2)").arg(doctorName).arg(doctorId));

    QVBoxLayout *lay = new QVBoxLayout(this);


    QHBoxLayout *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("患者ID:"));
    editPatientId_ = new QLineEdit(this);
    row1->addWidget(editPatientId_);

    row1->addWidget(new QLabel("科室:"));
    comboDept_ = new QComboBox(this);
    comboDept_->addItem("全部");
    comboDept_->addItem("内科");
    comboDept_->addItem("外科");
    comboDept_->addItem("儿科");
    comboDept_->addItem("骨科");
    comboDept_->addItem("急诊");
    row1->addWidget(comboDept_);

    row1->addWidget(new QLabel("关键词:"));
    editKeyword_ = new QLineEdit(this);
    row1->addWidget(editKeyword_);
    lay->addLayout(row1);


    QHBoxLayout *rowF = new QHBoxLayout;
    chkUseDate_ = new QCheckBox("使用日期范围", this);
    chkUseDate_->setChecked(false);
    rowF->addWidget(chkUseDate_);

    rowF->addWidget(new QLabel("起:"));
    dateFrom_ = new QDateEdit(this);
    dateFrom_->setCalendarPopup(true);
    dateFrom_->setDisplayFormat("yyyy-MM-dd");
    rowF->addWidget(dateFrom_);

    rowF->addWidget(new QLabel("止:"));
    dateTo_ = new QDateEdit(this);
    dateTo_->setCalendarPopup(true);
    dateTo_->setDisplayFormat("yyyy-MM-dd");
    rowF->addWidget(dateTo_);

    rowF->addWidget(new QLabel("每页:"));
    spinPageSize_ = new QSpinBox(this);
    spinPageSize_->setRange(1, 100);
    spinPageSize_->setValue(10);
    rowF->addWidget(spinPageSize_);

    btnPrev_ = new QPushButton("上一页", this);
    btnNext_ = new QPushButton("下一页", this);
    lblPage_  = new QLabel("第 1/1 页", this);
    rowF->addWidget(btnPrev_);
    rowF->addWidget(btnNext_);
    rowF->addWidget(lblPage_);
    lay->addLayout(rowF);


    editContent_ = new QPlainTextEdit(this);
    editContent_->setPlaceholderText("请输入医嘱/处方内容…");
    lay->addWidget(editContent_);

    QHBoxLayout *row2 = new QHBoxLayout;
    btnCreate_ = new QPushButton("开具医嘱", this);
    btnRefresh_ = new QPushButton("刷新该患者医嘱", this);
    row2->addWidget(btnCreate_);
    row2->addWidget(btnRefresh_);
    lay->addLayout(row2);

    table_ = new QTableWidget(0, 6, this);
    QStringList headers;
    headers << "ID" << "患者ID" << "医生ID" << "科室" << "摘要" << "时间";
    table_->setHorizontalHeaderLabels(headers);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    lay->addWidget(table_);

    connect(btnCreate_, SIGNAL(clicked()), this, SLOT(createOrder()));
    connect(btnRefresh_, SIGNAL(clicked()), this, SLOT(refreshOrders()));
    connect(btnPrev_,   SIGNAL(clicked()), this, SLOT(gotoPrev()));
    connect(btnNext_,   SIGNAL(clicked()), this, SLOT(gotoNext()));
    connect(table_, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onCellDoubleClicked(int,int)));
    connect(client_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));
}

void DoctorWindow::createOrder() {
    const QString p = editPatientId_->text().trimmed();
    const QString d = doctorId_;
    const QString dept = comboDept_->currentText() == "全部" ? "" : comboDept_->currentText();
    const QString content = editContent_->toPlainText().trimmed();

    if (p.isEmpty() || content.isEmpty()) {
        QMessageBox::information(this, "提示", "患者ID与医嘱内容必填。");
        return;
    }
    QJsonObject o;
    o.insert("type", "create_order");
    o.insert("patient_id", p);
    o.insert("doctor_id", d);
    o.insert("dept", dept);
    o.insert("content", content);
    client_->sendJson(o);
}

void DoctorWindow::refreshOrders() {
    currentPage_ = 1;
    sendListQuery();
}

void DoctorWindow::gotoPrev() {
    if (currentPage_ > 1) {
        currentPage_--;
        sendListQuery();
    }
}

void DoctorWindow::gotoNext() {
    int totalPages = (total_ + pageSize_ - 1) / pageSize_;
    if (currentPage_ < totalPages) {
        currentPage_++;
        sendListQuery();
    }
}

void DoctorWindow::sendListQuery() {
    const QString p = editPatientId_->text().trimmed();
    if (p.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入要查询的患者ID。");
        return;
    }
    pageSize_ = spinPageSize_->value();
    lastRequestedPatientId_ = p;

    QJsonObject o;
    o.insert("type", "list_orders");
    o.insert("patient_id", p);
    o.insert("keyword", editKeyword_->text().trimmed());
    o.insert("dept", comboDept_->currentText());


    if (chkUseDate_->isChecked()) {
        o.insert("date_from", dateFrom_->date().toString("yyyy-MM-dd"));
        o.insert("date_to",   dateTo_->date().toString("yyyy-MM-dd"));
    } else {
        o.insert("date_from", "");
        o.insert("date_to", "");
    }

    o.insert("page", currentPage_);
    o.insert("page_size", pageSize_);
    client_->sendJson(o);
}

void DoctorWindow::onJson(const QJsonObject &obj) {
    const QString type = obj.value("type").toString();
    if (type == "create_order_result") {
        if (obj.value("ok").toBool()) {
            QMessageBox::information(this, "成功", "已开具医嘱。");
            editContent_->clear();
            refreshOrders();
        } else {
            QMessageBox::warning(this, "失败", "开具失败：" + obj.value("error").toString());
        }
        return;
    }

    if (type == "orders") {

        if (obj.value("patient_id").toString() != lastRequestedPatientId_) return;

        total_ = obj.value("total").toInt();
        int page = obj.value("page").toInt();
        int pageSize = obj.value("page_size").toInt();
        currentPage_ = page;
        pageSize_ = pageSize;

        int totalPages = (total_ + pageSize_ - 1) / pageSize_;
        if (totalPages < 1) totalPages = 1;
        lblPage_->setText(QString("第 %1/%2 页（共 %3 条）").arg(currentPage_).arg(totalPages).arg(total_));
        btnPrev_->setEnabled(currentPage_ > 1);
        btnNext_->setEnabled(currentPage_ < totalPages);

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
        return;
    }

    if (type == "update_order_result" || type == "delete_order_result") {

        refreshOrders();
        return;
    }
}

void DoctorWindow::onCellDoubleClicked(int row, int) {
    if (row < 0 || row >= items_.size()) return;
    QJsonObject o = items_.at(row);
    OrderDetailDialog dlg(client_, o, true, this);
    connect(&dlg, SIGNAL(orderChanged()), this, SLOT(refreshOrders()));
    dlg.exec();
}
