#ifndef PATIENTWINDOW_H
#define PATIENTWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QJsonObject>
#include <QJsonArray>
#include "medlink.h"

//全局变量uID
//extern QString uID;

class PatientWindow : public QWidget {
    Q_OBJECT
public:
    explicit PatientWindow(MedLink *link, QWidget *parent = 0);

signals:
    void backRequested();

private slots:
    void refreshOrders();
    void onJson(const QJsonObject &obj);
    void onCellDoubleClicked(int row, int col);

private:
    MedLink *link_;
    QString patientId_;

    QPushButton *btnBack_;
    QLineEdit *editPatientId_;
    QPushButton *btnRefresh_;
    QTableWidget *table_;

    QList<QJsonObject> items_;
};

#endif
