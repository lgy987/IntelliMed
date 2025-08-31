#ifndef PATIENTWINDOW_H
#define PATIENTWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QJsonObject>
#include <QJsonArray>
#include "client.h"

class PatientWindow : public QWidget {
    Q_OBJECT
public:
    explicit PatientWindow(Client *client, const QString &patientId, QWidget *parent = 0);

private slots:
    void refreshOrders();
    void onJson(const QJsonObject &obj);
    void onCellDoubleClicked(int row, int col);

private:
    Client *client_;
    QString patientId_;
    QLineEdit *editPatientId_;
    QPushButton *btnRefresh_;
    QTableWidget *table_;

    QList<QJsonObject> items_;
};

#endif
