#include "orderdetaildialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonArray>

OrderDetailDialog::OrderDetailDialog(Client *client,
                                     const QJsonObject &orderObj,
                                     bool editable,
                                     QWidget *parent)
    : QDialog(parent), client_(client), data_(orderObj), editable_(editable)
{
    setWindowTitle("医嘱详情");
    orderId_ = orderObj.value("id").toInt();

    QVBoxLayout *lay = new QVBoxLayout(this);

    QHBoxLayout *r1 = new QHBoxLayout;
    r1->addWidget(new QLabel("ID:"));
    editId_ = new QLineEdit(QString::number(orderId_), this);
    editId_->setReadOnly(true);
    r1->addWidget(editId_);

    r1->addWidget(new QLabel("患者ID:"));
    editPatient_ = new QLineEdit(orderObj.value("patient_id").toString(), this);
    editPatient_->setReadOnly(true);
    r1->addWidget(editPatient_);

    r1->addWidget(new QLabel("医生ID:"));
    editDoctor_ = new QLineEdit(orderObj.value("doctor_id").toString(), this);
    editDoctor_->setReadOnly(true);
    r1->addWidget(editDoctor_);
    lay->addLayout(r1);

    QHBoxLayout *r2 = new QHBoxLayout;
    r2->addWidget(new QLabel("科室:"));
    comboDept_ = new QComboBox(this);
    comboDept_->addItem("内科");
    comboDept_->addItem("外科");
    comboDept_->addItem("儿科");
    comboDept_->addItem("骨科");
    comboDept_->addItem("急诊");
    comboDept_->addItem("全部"); // 给个兜底
    int idx = comboDept_->findText(orderObj.value("dept").toString());
    if (idx < 0) idx = comboDept_->findText("全部");
    comboDept_->setCurrentIndex(idx);
    r2->addWidget(comboDept_);

    r2->addWidget(new QLabel("创建时间:"));
    editCreatedAt_ = new QLineEdit(orderObj.value("created_at").toString(), this);
    editCreatedAt_->setReadOnly(true);
    r2->addWidget(editCreatedAt_);
    lay->addLayout(r2);

    editContent_ = new QPlainTextEdit(orderObj.value("content").toString(), this);
    editContent_->setPlaceholderText("医嘱内容");
    lay->addWidget(editContent_);

    QHBoxLayout *btns = new QHBoxLayout;
    btnSave_ = new QPushButton("保存修改", this);
    btnDelete_ = new QPushButton("删除", this);
    btnClose_ = new QPushButton("关闭", this);
    btns->addWidget(btnSave_);
    btns->addWidget(btnDelete_);
    btns->addStretch(1);
    btns->addWidget(btnClose_);
    lay->addLayout(btns);

    if (!editable_) {
        btnSave_->setEnabled(false);
        btnDelete_->setEnabled(false);
        comboDept_->setEnabled(false);
        editContent_->setReadOnly(true);
        setWindowTitle("医嘱详情（只读）");
    }

    connect(btnSave_, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(btnDelete_, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(btnClose_, SIGNAL(clicked()), this, SLOT(close()));
    connect(client_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));
}

void OrderDetailDialog::onSave() {
    if (!editable_) return;
    QJsonObject o;
    o.insert("type", "update_order");
    o.insert("id", orderId_);
    o.insert("dept", comboDept_->currentText());
    o.insert("content", editContent_->toPlainText());
    client_->sendJson(o);
}

void OrderDetailDialog::onDelete() {
    if (!editable_) return;
    if (QMessageBox::question(this, "确认删除", "确定要删除这条医嘱吗？") != QMessageBox::Yes) return;
    QJsonObject o;
    o.insert("type", "delete_order");
    o.insert("id", orderId_);
    client_->sendJson(o);
}

void OrderDetailDialog::onJson(const QJsonObject &obj) {
    QString type = obj.value("type").toString();
    if (type == "update_order_result") {
        if (obj.value("id").toInt() != orderId_) return;
        bool ok = obj.value("ok").toBool();
        if (ok) {
            QMessageBox::information(this, "成功", "已保存修改。");
            emit orderChanged();
            accept();
        } else {
            QMessageBox::warning(this, "失败", "保存失败：" + obj.value("error").toString());
        }
    } else if (type == "delete_order_result") {
        if (obj.value("id").toInt() != orderId_) return;
        bool ok = obj.value("ok").toBool();
        if (ok) {
            QMessageBox::information(this, "成功", "已删除。");
            emit orderChanged();
            accept();
        } else {
            QMessageBox::warning(this, "失败", "删除失败：" + obj.value("error").toString());
        }
    }
}

