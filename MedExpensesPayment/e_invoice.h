#ifndef E_INVOICE_H
#define E_INVOICE_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class E_Invoice;
}

class Database;

class E_Invoice : public QWidget
{
    Q_OBJECT

public:
    explicit E_Invoice(Database *db = nullptr, QWidget *parent = nullptr);
    ~E_Invoice();
    
    // 公共方法，供外部调用
    void refreshData();

signals:
    void backRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void generateInvoice();
    void printInvoice();
    void updateSummary();
    void downloadData();           // 新增：下载功能
    void applyFilters();           // 新增：应用筛选功能
    void resetFilters();           // 新增：重置筛选功能

private:
    Ui::E_Invoice *ui;
    Database *database;
    QSqlTableModel *model;
    
    void setupTableView();
    void connectSignals();
    void setupDatabase();
    void setupFilters();           // 新增：设置筛选控件
};

#endif // E_INVOICE_H
