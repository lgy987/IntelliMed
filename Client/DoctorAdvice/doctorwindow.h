#ifndef DOCTORWINDOW_H
#define DOCTORWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QJsonObject>
#include <QJsonArray>
#include "medlink.h"

// 全局用户ID
//extern QString uID;

class DoctorWindow : public QWidget {
    Q_OBJECT
public:
    // 登录已删除：直接用全局 uID 当医生ID
    explicit DoctorWindow(MedLink *link, QWidget *parent = 0);

signals:
    void backRequested();

private slots:
    void createOrder();
    void refreshOrders();
    void onJson(const QJsonObject &obj);
    void onCellDoubleClicked(int row, int col);
    void gotoPrev();
    void gotoNext();

private:
    void sendListQuery();

private:
    MedLink *link_;

    QPushButton *btnBack_;

    QLineEdit *editDoctorId_;    // 由 uID 预填（只读）
    QLineEdit *editPatientId_;   // 查询患者ID（可编辑）
    QComboBox *comboDept_;
    QLineEdit *editKeyword_;
    QCheckBox *chkUseDate_;
    QDateEdit *dateFrom_;
    QDateEdit *dateTo_;
    QSpinBox  *spinPageSize_;
    QPushButton *btnPrev_;
    QPushButton *btnNext_;
    QLabel *lblPage_;

    QPlainTextEdit *editContent_;
    QPushButton *btnCreate_;
    QPushButton *btnRefresh_;
    QTableWidget *table_;

    int currentPage_;
    int total_;
    int pageSize_;
    QList<QJsonObject> items_;
    QString lastRequestedPatientId_;
};

#endif
