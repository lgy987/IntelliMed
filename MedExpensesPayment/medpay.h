#ifndef MEDPAY_H
#define MEDPAY_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MedPay; }
QT_END_NAMESPACE

class Unpaid;
class PaymentHistory;
class E_Invoice;
class Database;

class MedPay : public QWidget
{
    Q_OBJECT

public:
    MedPay(QWidget *parent = nullptr);
    ~MedPay();

private slots:
    void updateUserInfo();
    void refreshData();

private:
    Ui::MedPay *ui;
    Unpaid *unpaidWindow;
    PaymentHistory *historyWindow;
    E_Invoice *invoiceWindow;
    Database *database;
    
    void setupDatabase();
    void loadUserInfo();
};
#endif // MEDPAY_H
