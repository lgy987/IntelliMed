#include "orderdetaildialog.h"
#include "ui_theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QJsonArray>
#include <QGroupBox>
#include <QGridLayout>

OrderDetailDialog::OrderDetailDialog(MedLink *client,
                                     const QJsonObject &orderObj,
                                     bool editable,
                                     QWidget *parent)
    : QDialog(parent), client_(client), data_(orderObj), editable_(editable)
{
    setWindowTitle(editable ? "医嘱详情 / 编辑" : "医嘱详情（只读）");
    resize(760, 540);
    UiTheme::apply(this);

    orderId_ = orderObj.value("id").toInt();

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(18,18,18,18);
    lay->setSpacing(14);

    QGroupBox *info = new QGroupBox("基本信息", this);
    QGridLayout *g = new QGridLayout(info); g->setHorizontalSpacing(12); g->setVerticalSpacing(10);

    g->addWidget(new QLabel("ID:"), 0, 0);
    editId_ = new QLineEdit(QString::number(orderId_), this); editId_->setReadOnly(true);
    g->addWidget(editId_, 0, 1);

    g->addWidget(new QLabel("患者ID:"), 0, 2);
    editPatient_ = new QLineEdit(orderObj.value("patient_id").toString(), this); editPatient_->setReadOnly(true);
    g->addWidget(editPatient_, 0, 3);

    g->addWidget(new QLabel("医生ID:"), 1, 0);
    editDoctor_ = new QLineEdit(orderObj.value("doctor_id").toString(), this); editDoctor_->setReadOnly(true);
    g->addWidget(editDoctor_, 1, 1);

    g->addWidget(new QLabel("创建时间:"), 1, 2);
    editCreatedAt_ = new QLineEdit(orderObj.value("created_at").toString(), this); editCreatedAt_->setReadOnly(true);
    g->addWidget(editCreatedAt_, 1, 3);

    g->addWidget(new QLabel("科室:"), 2, 0);
    comboDept_ = new QComboBox(this);
    comboDept_->addItems(QStringList() << "内科" << "外科" << "儿科" << "骨科" << "急诊" << "全部");
    int idx = comboDept_->findText(orderObj.value("dept").toString()); if (idx < 0) idx = comboDept_->findText("全部");
    comboDept_->setCurrentIndex(idx);
    g->addWidget(comboDept_, 2, 1, 1, 3);

    lay->addWidget(info);

    QGroupBox *contentBox = new QGroupBox("医嘱内容", this);
    QVBoxLayout *cl = new QVBoxLayout(contentBox);
    editContent_ = new QPlainTextEdit(this); editContent_->setPlaceholderText("医嘱内容");
    editContent_->setPlainText(orderObj.value("content").toString());
    cl->addWidget(editContent_);
    lay->addWidget(contentBox, 1);

    QHBoxLayout *btns = new QHBoxLayout;
    btnSave_ = new QPushButton("保存修改", this);
    btnDelete_ = new QPushButton("删除", this); btnDelete_->setObjectName("danger");
    btnClose_ = new QPushButton("关闭", this); btnClose_->setObjectName("secondary");
    btns->addWidget(btnSave_); btns->addWidget(btnDelete_); btns->addStretch(1); btns->addWidget(btnClose_);
    lay->addLayout(btns);

    if (!editable_) {
        btnSave_->setEnabled(false);
        btnDelete_->setEnabled(false);
        comboDept_->setEnabled(false);
        editContent_->setReadOnly(true);
    }

    connect(btnSave_, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(btnDelete_, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(btnClose_, SIGNAL(clicked()), this, SLOT(close()));
    connect(client_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));
}

void OrderDetailDialog::onSave() {
    if (!editable_) return;
    QJsonObject o; o.insert("type","update_order"); o.insert("id", orderId_);
    o.insert("dept", comboDept_->currentText()); o.insert("content", editContent_->toPlainText());
    client_->sendJson(o);
}

void OrderDetailDialog::onDelete() {
    if (!editable_) return;
    if (QMessageBox::question(this, "确认删除", "确定要删除这条医嘱吗？") != QMessageBox::Yes) return;
    QJsonObject o; o.insert("type","delete_order"); o.insert("id", orderId_);
    client_->sendJson(o);
}

void OrderDetailDialog::onJson(const QJsonObject &obj) {
    QString type = obj.value("type").toString();
    if (type == "update_order_result") {
        if (obj.value("id").toInt() != orderId_) return;
        if (obj.value("ok").toBool()) { QMessageBox::information(this,"成功","已保存修改。"); emit orderChanged(); accept(); }
        else { QMessageBox::warning(this,"失败","保存失败："+obj.value("error").toString()); }
    } else if (type == "delete_order_result") {
        if (obj.value("id").toInt() != orderId_) return;
        if (obj.value("ok").toBool()) { QMessageBox::information(this,"成功","已删除。"); emit orderChanged(); accept(); }
        else { QMessageBox::warning(this,"失败","删除失败："+obj.value("error").toString()); }
    }
}
