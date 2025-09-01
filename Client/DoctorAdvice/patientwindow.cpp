#include "patientwindow.h"
#include "orderdetaildialog.h"
#include "ui_theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>
#include "session.h"

//全局变量uID
//extern QString uID;

PatientWindow::PatientWindow(MedLink *link, QWidget *parent)
    : QWidget(parent), link_(link)
{
    setWindowTitle("患者端");
    UiTheme::apply(this);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    QFrame *appbar = new QFrame(this);
    appbar->setObjectName("appBar");
    QHBoxLayout *head = new QHBoxLayout(appbar);
    QLabel *title = new QLabel("我的医嘱", appbar);
    QFont f = title->font(); f.setPointSize(18); f.setBold(true); title->setFont(f);
    head->addWidget(title); head->addStretch(1);
    btnBack_ = new QPushButton("返回", appbar); btnBack_->setObjectName("ghost");
    head->addWidget(btnBack_);
    root->addWidget(appbar);
    connect(btnBack_, SIGNAL(clicked()), this, SIGNAL(backRequested()));

    QGroupBox *bar = new QGroupBox("查询", this);
    QHBoxLayout *barLay = new QHBoxLayout(bar);
    barLay->addWidget(new QLabel("患者ID:"));
    editPatientId_ = new QLineEdit(bar);
    editPatientId_->setPlaceholderText("请输入患者ID，例如 p001");
    QString uID = Session::instance().userId(); // 全局 uID
    qDebug() << "id:" << uID;
    editPatientId_->setText(uID); // 用全局 uID 预填
    barLay->addWidget(editPatientId_);
    btnRefresh_ = new QPushButton("刷新医嘱", bar);
    btnRefresh_->setObjectName("secondary");
    barLay->addWidget(btnRefresh_);
    barLay->addStretch(1);
    root->addWidget(bar);

    QGroupBox *list = new QGroupBox("医嘱列表", this);
    QVBoxLayout *ll = new QVBoxLayout(list);
    table_ = new QTableWidget(0, 6, list);
    QStringList headers; headers << "ID" << "患者ID" << "医生ID" << "科室" << "摘要" << "时间";
    table_->setHorizontalHeaderLabels(headers);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    UiTheme::styleTable(table_);
    ll->addWidget(table_);
    root->addWidget(list, 1);

    connect(btnRefresh_, SIGNAL(clicked()), this, SLOT(refreshOrders()));
    connect(table_, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onCellDoubleClicked(int,int)));
    connect(link_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));


    refreshOrders();
}

void PatientWindow::refreshOrders() {
    patientId_ = editPatientId_->text().trimmed();
    if (patientId_.isEmpty()) { QMessageBox::information(this,"提示","请输入患者ID。"); return; }

    QJsonObject o; o.insert("type","list_orders"); o.insert("patient_id", patientId_);
    o.insert("keyword",""); o.insert("dept","全部"); o.insert("date_from",""); o.insert("date_to","");
    o.insert("page",1); o.insert("page_size",50);
    link_->sendJson(o);
}

void PatientWindow::onJson(const QJsonObject &obj) {
    const QString type = obj.value("type").toString();
    if (type == "orders" && obj.value("patient_id").toString() == patientId_) {
        QJsonArray items = obj.value("items").toArray();
        items_.clear(); table_->setRowCount(items.size());
        int row = 0;
        for (int i=0;i<items.size();++i) {
            QJsonObject o = items.at(i).toObject(); items_.append(o);
            table_->setItem(row,0,new QTableWidgetItem(QString::number(o.value("id").toInt())));
            table_->setItem(row,1,new QTableWidgetItem(o.value("patient_id").toString()));
            table_->setItem(row,2,new QTableWidgetItem(o.value("doctor_id").toString()));
            table_->setItem(row,3,new QTableWidgetItem(o.value("dept").toString()));
            QString content = o.value("content").toString(); QString summary = content.left(24);
            if (content.length()>24) summary += "…";
            table_->setItem(row,4,new QTableWidgetItem(summary));
            table_->setItem(row,5,new QTableWidgetItem(o.value("created_at").toString()));
            ++row;
        }
    }
}

void PatientWindow::onCellDoubleClicked(int row, int) {
    if (row < 0 || row >= items_.size()) return;
    QJsonObject o = items_.at(row);
    OrderDetailDialog dlg(link_, o, false, this);
    dlg.exec();
}
