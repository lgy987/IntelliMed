#ifndef CASEDATABASE_H
#define CASEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QDir>

class CaseDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CaseDatabase(QObject *parent = nullptr);
    ~CaseDatabase();

    bool initDatabase();
    QJsonArray queryCasesByVirus(const QString &virus, bool isDoctorView);
    QJsonArray queryAllCases();
    bool updateCase(const QJsonObject &caseData); // 添加到public部分

private:
    QSqlDatabase m_db;

    bool createTables();
    bool insertSampleData();
};
#endif // CASEDATABASE_H
