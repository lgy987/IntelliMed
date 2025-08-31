#include "medpay.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MedPay w;
    w.show();
    return a.exec();
}
