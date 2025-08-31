#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QDebug>
#include <QDateTime>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    // 数据库连接管理
    bool connectDatabase();
    void closeDatabase();
    bool isConnected() const;

    // 表操作
    bool createTables();
    bool dropTables();

    // 医疗费用记录操作
    bool insertMedicalExpense(const QString &itemName, double unitPrice, int quantity, 
                             double amount, const QString &department, 
                             const QString &paymentMethod, const QString &paymentStatus);
    
    bool updatePaymentStatus(int id, const QString &status);
    bool deleteMedicalExpense(int id);
    
    // 查询操作
    QSqlTableModel* getUnpaidExpenses();
    QSqlTableModel* getPaymentHistory();
    QSqlTableModel* getInvoiceList();
    
    // 统计信息
    double getTotalUnpaidAmount();
    int getUnpaidCount();
    double getTotalPaidAmount();
    int getPaidCount();

signals:
    void dataChanged(); // 数据变化信号

private:
    QSqlDatabase m_db;
    bool m_connected;

    // 创建医疗费用表
    bool createMedicalExpensesTable();
    
    // 插入示例数据
    bool insertSampleData();
};

#endif // DATABASE_H
