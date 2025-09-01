#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class Database : public QObject {
    Q_OBJECT
public:
    explicit Database(QObject *parent = 0);

    bool open();
    void ensureSchema();
    void seed();

    QString queryUserRole(const QString &userId, QString *nameOut = 0);

    bool createOrder(const QString &patientId,
                     const QString &doctorId,
                     const QString &dept,
                     const QString &content,
                     QString *err = 0);


    int countOrders(const QString &patientId,
                    const QString &keyword,
                    const QString &dept,
                    const QString &dateFromIso,
                    const QString &dateToIso);

    QString listOrdersAsJson(const QString &patientId,
                             const QString &keyword,
                             const QString &dept,
                             const QString &dateFromIso,
                             const QString &dateToIso,
                             int page,
                             int pageSize);


    bool updateOrder(int id, const QString &dept, const QString &content, QString *err = 0);
    bool deleteOrder(int id, QString *err = 0);

private:
    QSqlDatabase db_;
    bool execInit(const QString &sql);
};

#endif
