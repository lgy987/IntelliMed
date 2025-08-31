#ifndef PAYMENTHISTORY_H
#define PAYMENTHISTORY_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class PaymentHistory;
}

class Database;

class PaymentHistory : public QWidget
{
    Q_OBJECT

public:
    explicit PaymentHistory(Database *db = nullptr, QWidget *parent = nullptr);
    ~PaymentHistory();
    
    // 公共方法，供外部调用
    void refreshData();

signals:
    void backRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void exportData();
    void printData();      // 新增：打印功能
    void updateSummary();

private:
    Ui::PaymentHistory *ui;
    Database *database;
    QSqlTableModel *model;
    
    void setupTableView();
    void connectSignals();
    void setupDatabase();
};

#endif // PAYMENTHISTORY_H
