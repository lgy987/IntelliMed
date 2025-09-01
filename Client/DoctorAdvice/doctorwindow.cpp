#include "doctorwindow.h"
#include "orderdetaildialog.h"
#include "ui_theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QSplitter>
#include <QGroupBox>
#include <QFrame>
#include <QGridLayout>
#include "doctorsession.h"

// 全局变量声明
//extern QString uID;

DoctorWindow::DoctorWindow(MedLink *link, QWidget *parent)
    : QWidget(parent), link_(link), currentPage_(1), total_(0), pageSize_(10)
{
    setWindowTitle("医生工作台");
    UiTheme::apply(this);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(20,20,20,20);
    root->setSpacing(16);


    QFrame *appbar = new QFrame(this);
    appbar->setObjectName("appBar");
    QHBoxLayout *top = new QHBoxLayout(appbar);
    QLabel *title = new QLabel("医嘱管理", appbar);
    QFont f = title->font(); f.setPointSize(20); f.setBold(true); title->setFont(f);
    top->addWidget(title);
    top->addStretch(1);
    btnBack_ = new QPushButton("返回", appbar);
    btnBack_->setObjectName("ghost");
    top->addWidget(btnBack_);
    root->addWidget(appbar);
    connect(btnBack_, SIGNAL(clicked()), this, SIGNAL(backRequested()));

    QSplitter *split = new QSplitter(Qt::Horizontal, this);
    split->setHandleWidth(12);
    root->addWidget(split, 1);


    QWidget *left = new QWidget(split);
    left->setObjectName("sideBar");
    QVBoxLayout *lv = new QVBoxLayout(left); lv->setSpacing(12); lv->setContentsMargins(12,12,12,12);

    QGroupBox *filter = new QGroupBox("筛选与操作", left);
    QVBoxLayout *fl = new QVBoxLayout(filter); fl->setSpacing(10);
    QGridLayout *fg = new QGridLayout; fg->setHorizontalSpacing(12); fg->setVerticalSpacing(10);

    fg->addWidget(new QLabel("医生ID"), 0, 0);
    editDoctorId_ = new QLineEdit(filter);
    QString uID = DoctorSession::instance().userId(); // 全局 uID
    editDoctorId_->setText(uID);         // 由全局 uID 预填
    editDoctorId_->setReadOnly(true);
    fg->addWidget(editDoctorId_, 0, 1);

    fg->addWidget(new QLabel("患者ID"), 1, 0);
    editPatientId_ = new QLineEdit(filter);
    editPatientId_->setPlaceholderText("必填：例如 p001（用于查询/开具）");
    fg->addWidget(editPatientId_, 1, 1);

    fg->addWidget(new QLabel("科室"), 2, 0);
    comboDept_ = new QComboBox(filter);
    comboDept_->addItems(QStringList() << "全部" << "内科" << "外科" << "儿科" << "骨科" << "急诊");
    fg->addWidget(comboDept_, 2, 1);

    fg->addWidget(new QLabel("关键词"), 3, 0);
    editKeyword_ = new QLineEdit(filter); editKeyword_->setPlaceholderText("按内容/科室模糊匹配");
    fg->addWidget(editKeyword_, 3, 1);

    chkUseDate_ = new QCheckBox("使用日期范围", filter);
    fl->addLayout(fg);
    fl->addWidget(chkUseDate_);

    QHBoxLayout *dates = new QHBoxLayout;
    dates->addWidget(new QLabel("起"));
    dateFrom_ = new QDateEdit(filter); dateFrom_->setCalendarPopup(true); dateFrom_->setDisplayFormat("yyyy-MM-dd");
    dates->addWidget(dateFrom_);
    dates->addSpacing(8);
    dates->addWidget(new QLabel("止"));
    dateTo_ = new QDateEdit(filter); dateTo_->setCalendarPopup(true); dateTo_->setDisplayFormat("yyyy-MM-dd");
    dates->addWidget(dateTo_);
    dates->addStretch(1);
    fl->addLayout(dates);

    QHBoxLayout *pager = new QHBoxLayout;
    pager->addWidget(new QLabel("每页"));
    spinPageSize_ = new QSpinBox(filter); spinPageSize_->setRange(1,100); spinPageSize_->setValue(10);
    pager->addWidget(spinPageSize_);
    btnPrev_ = new QPushButton("上一页", filter); btnPrev_->setObjectName("secondary");
    btnNext_ = new QPushButton("下一页", filter);
    lblPage_  = new QLabel("第 1/1 页", filter); lblPage_->setProperty("hint", true);
    pager->addStretch(1);
    pager->addWidget(btnPrev_); pager->addWidget(btnNext_); pager->addWidget(lblPage_);
    fl->addLayout(pager);

    QHBoxLayout *act = new QHBoxLayout;
    btnRefresh_ = new QPushButton("刷新该患者医嘱", filter); btnRefresh_->setObjectName("secondary");
    act->addWidget(btnRefresh_); act->addStretch(1);
    fl->addLayout(act);

    QGroupBox *create = new QGroupBox("开具医嘱", left);
    QVBoxLayout *cl = new QVBoxLayout(create);
    editContent_ = new QPlainTextEdit(create); editContent_->setPlaceholderText("请输入医嘱/处方内容…");
    btnCreate_ = new QPushButton("开具医嘱", create);
    cl->addWidget(editContent_);
    cl->addWidget(btnCreate_);

    left->setMinimumWidth(420);
    lv->addWidget(filter);
    lv->addWidget(create, 1);


    QWidget *right = new QWidget(split);
    QVBoxLayout *rv = new QVBoxLayout(right); rv->setContentsMargins(0,0,0,0); rv->setSpacing(12);

    QGroupBox *list = new QGroupBox("医嘱列表", right);
    QVBoxLayout *ll = new QVBoxLayout(list);
    table_ = new QTableWidget(0, 6, list);
    QStringList headers; headers << "ID" << "患者ID" << "医生ID" << "科室" << "摘要" << "时间";
    table_->setHorizontalHeaderLabels(headers);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    UiTheme::styleTable(table_);
    ll->addWidget(table_);
    rv->addWidget(list, 1);

    split->addWidget(left);
    split->addWidget(right);
    split->setStretchFactor(0, 0);
    split->setStretchFactor(1, 1);


    connect(btnCreate_, SIGNAL(clicked()), this, SLOT(createOrder()));
    connect(btnRefresh_, SIGNAL(clicked()), this, SLOT(refreshOrders()));
    connect(btnPrev_,   SIGNAL(clicked()), this, SLOT(gotoPrev()));
    connect(btnNext_,   SIGNAL(clicked()), this, SLOT(gotoNext()));
    connect(table_, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onCellDoubleClicked(int,int)));
    connect(link_, SIGNAL(jsonReceived(QJsonObject)), this, SLOT(onJson(QJsonObject)));
}

void DoctorWindow::createOrder() {
    const QString doctorId = editDoctorId_->text().trimmed();
    const QString patientId = editPatientId_->text().trimmed();
    const QString dept = comboDept_->currentText() == "全部" ? "" : comboDept_->currentText();
    const QString content = editContent_->toPlainText().trimmed();
    if (doctorId.isEmpty() || patientId.isEmpty() || content.isEmpty()) {
        QMessageBox::information(this,"提示","医生ID、患者ID、医嘱内容均为必填。"); return;
    }
    QJsonObject o; o.insert("type","create_order");
    o.insert("patient_id", patientId);
    o.insert("doctor_id", doctorId);
    o.insert("dept", dept);
    o.insert("content", content);
    link_->sendJson(o);
}

void DoctorWindow::refreshOrders() { currentPage_ = 1; sendListQuery(); }
void DoctorWindow::gotoPrev() { if (currentPage_ > 1){ currentPage_--; sendListQuery(); } }
void DoctorWindow::gotoNext() { int totalPages = (total_ + pageSize_ - 1)/pageSize_; if (currentPage_ < totalPages){ currentPage_++; sendListQuery(); } }

void DoctorWindow::sendListQuery() {
    const QString p = editPatientId_->text().trimmed();
    pageSize_ = spinPageSize_->value();
    if (p.isEmpty()) { QMessageBox::information(this,"提示","请输入要查询的患者ID。"); return; }
    lastRequestedPatientId_ = p;

    QJsonObject o; o.insert("type","list_orders"); o.insert("patient_id",p);
    o.insert("keyword", editKeyword_->text().trimmed());
    o.insert("dept", comboDept_->currentText());
    if (chkUseDate_->isChecked()) { o.insert("date_from", dateFrom_->date().toString("yyyy-MM-dd"));
                                    o.insert("date_to",   dateTo_->date().toString("yyyy-MM-dd")); }
    else { o.insert("date_from",""); o.insert("date_to",""); }
    o.insert("page", currentPage_); o.insert("page_size", pageSize_);
    link_->sendJson(o);
}

void DoctorWindow::onJson(const QJsonObject &obj) {
    const QString type = obj.value("type").toString();
    if (type == "create_order_result") {
        if (obj.value("ok").toBool()) { QMessageBox::information(this,"成功","已开具医嘱。"); editContent_->clear(); refreshOrders(); }
        else { QMessageBox::warning(this,"失败","开具失败："+obj.value("error").toString()); }
        return;
    }
    if (type == "orders") {
        if (obj.value("patient_id").toString() != lastRequestedPatientId_) return;
        total_ = obj.value("total").toInt();
        int page = obj.value("page").toInt();
        int pageSize = obj.value("page_size").toInt();
        currentPage_ = page; pageSize_ = pageSize;
        int totalPages = (total_ + pageSize_ - 1)/pageSize_; if (totalPages < 1) totalPages = 1;
        lblPage_->setText(QString("第 %1/%2 页（共 %3 条）").arg(currentPage_).arg(totalPages).arg(total_));
        btnPrev_->setEnabled(currentPage_ > 1); btnNext_->setEnabled(currentPage_ < totalPages);

        QJsonArray items = obj.value("items").toArray();
        items_.clear(); table_->setRowCount(items.size());
        int row = 0;
        for (int i=0;i<items.size();++i){
            QJsonObject o = items.at(i).toObject(); items_.append(o);
            table_->setItem(row,0,new QTableWidgetItem(QString::number(o.value("id").toInt())));
            table_->setItem(row,1,new QTableWidgetItem(o.value("patient_id").toString()));
            table_->setItem(row,2,new QTableWidgetItem(o.value("doctor_id").toString()));
            table_->setItem(row,3,new QTableWidgetItem(o.value("dept").toString()));
            QString content = o.value("content").toString(); QString summary = content.left(28);
            if (content.length()>28) summary += "…";
            table_->setItem(row,4,new QTableWidgetItem(summary));
            table_->setItem(row,5,new QTableWidgetItem(o.value("created_at").toString()));
            ++row;
        }
        return;
    }
    if (type == "update_order_result" || type == "delete_order_result") { refreshOrders(); return; }
}

void DoctorWindow::onCellDoubleClicked(int row, int) {
    if (row < 0 || row >= items_.size()) return;
    QJsonObject o = items_.at(row);
    OrderDetailDialog dlg(link_, o, true, this);
    connect(&dlg, SIGNAL(orderChanged()), this, SLOT(refreshOrders()));
    dlg.exec();
}
