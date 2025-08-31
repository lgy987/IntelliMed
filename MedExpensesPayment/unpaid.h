#ifndef UNPAID_H
#define UNPAID_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class Unpaid;
}

class Database;

class Unpaid : public QWidget
{
    Q_OBJECT

public:
    explicit Unpaid(Database *db = nullptr, QWidget *parent = nullptr);
    ~Unpaid();
    
    // 公共方法，供外部调用
    void refreshData();

signals:
    void backRequested();

private slots:
    void insertData(); // 新增插入数据函数
    void paySelected();
    void payAll();
    void updateSummary();

private:
    Ui::Unpaid *ui;
    Database *database;
    QSqlTableModel *model;
    
    void setupTableView();
    void connectSignals();
};

#endif // UNPAID_H
