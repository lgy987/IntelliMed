#ifndef ORDERDETAILDIALOG_H
#define ORDERDETAILDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QJsonObject>
#include "client.h"

class OrderDetailDialog : public QDialog {
    Q_OBJECT
public:
    explicit OrderDetailDialog(Client *client,
                               const QJsonObject &orderObj,
                               bool editable,
                               QWidget *parent = 0);

signals:
    void orderChanged();
private slots:
    void onSave();
    void onDelete();
    void onJson(const QJsonObject &obj);

private:
    Client *client_;
    QJsonObject data_;
    bool editable_;
    int orderId_;

    QLineEdit *editId_;
    QLineEdit *editPatient_;
    QLineEdit *editDoctor_;
    QLineEdit *editCreatedAt_;
    QComboBox *comboDept_;
    QPlainTextEdit *editContent_;
    QPushButton *btnSave_;
    QPushButton *btnDelete_;
    QPushButton *btnClose_;
};

#endif
