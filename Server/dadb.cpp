#include "dadb.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

Database::Database(QObject *parent) : QObject(parent) { ensureSchema(); }

bool Database::open() {
    if (QSqlDatabase::contains("medical")) {
        db_ = QSqlDatabase::database("medical");
    } else {
        db_ = QSqlDatabase::addDatabase("QSQLITE", "medical");
        const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(path);
        db_.setDatabaseName(path + "/medical.db");
    }
    return db_.open();
}

bool Database::execInit(const QString &sql) {
    QSqlQuery q(db_);
    if (!q.exec(sql)) {
        qWarning() << "SQL error:" << q.lastError().text() << " in " << sql;
        return false;
    }
    return true;
}

void Database::ensureSchema() {
    execInit("PRAGMA foreign_keys = ON;");

    QString sql1;
    sql1 += "CREATE TABLE IF NOT EXISTS dausers(";
    sql1 += "  id TEXT PRIMARY KEY,";
    sql1 += "  name TEXT,";
    sql1 += "  role TEXT CHECK(role IN ('doctor','patient')) NOT NULL";
    sql1 += ");";
    execInit(sql1);

    QString sql2;
    sql2 += "CREATE TABLE IF NOT EXISTS daorders(";
    sql2 += "  id INTEGER PRIMARY KEY AUTOINCREMENT,";
    sql2 += "  patient_id TEXT NOT NULL,";
    sql2 += "  doctor_id  TEXT NOT NULL,";
    sql2 += "  dept       TEXT,";
    sql2 += "  content    TEXT,";
    sql2 += "  created_at TEXT NOT NULL";
    sql2 += ");";
    execInit(sql2);
}

void Database::seed() {
    QString ins;
    ins += "INSERT OR IGNORE INTO dausers(id,name,role) VALUES";
    ins += "('d001','张医生','doctor'),";
    ins += "('p001','李患者','patient');";
    execInit(ins);
}

QString Database::queryUserRole(const QString &userId, QString *nameOut) {
    QSqlQuery q(db_);
    q.prepare("SELECT name, role FROM dausers WHERE id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) {
        if (nameOut) *nameOut = q.value(0).toString();
        return q.value(1).toString();
    }
    return QString();
}

bool Database::createOrder(const QString &patientId,
                           const QString &doctorId,
                           const QString &dept,
                           const QString &content,
                           QString *err) {
    qDebug() << patientId << doctorId<<dept<<content;
    QSqlQuery q(db_);
    q.prepare("INSERT INTO daorders(patient_id,doctor_id,dept,content,created_at) "
              "VALUES(?,?,?,?,?)");
    q.addBindValue(patientId);
    q.addBindValue(doctorId);
    q.addBindValue(dept);
    q.addBindValue(content);
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate);
    if (!q.exec()) {
        if (err) *err = q.lastError().text();
        qDebug() << q.lastError().text();
        return false;
    }
    return true;
}

static void buildWhere(QString &sql, QList<QVariant> &binds,
                       const QString &patientId,
                       const QString &keyword,
                       const QString &dept,
                       const QString &dateFromIso,
                       const QString &dateToIso)
{
    sql += " WHERE 1=1";
    if (!patientId.isEmpty()) {
        sql += " AND patient_id = ?";
        binds.append(patientId);
    }
    if (!keyword.isEmpty()) {
        sql += " AND (content LIKE ? OR dept LIKE ?)";
        QString pat = "%" + keyword + "%";
        binds.append(pat);
        binds.append(pat);
    }
    if (!dept.isEmpty() && dept != "全部") {
        sql += " AND dept = ?";
        binds.append(dept);
    }
    if (!dateFromIso.isEmpty()) {

        sql += " AND created_at >= ?";
        QString fromIso = dateFromIso + "T00:00:00";
        binds.append(fromIso);
    }
    if (!dateToIso.isEmpty()) {
        sql += " AND created_at <= ?";
        QString toIso = dateToIso + "T23:59:59";
        binds.append(toIso);
    }
}

int Database::countOrders(const QString &patientId,
                          const QString &keyword,
                          const QString &dept,
                          const QString &dateFromIso,
                          const QString &dateToIso) {
    QString sql = "SELECT COUNT(*) FROM daorders";
    QList<QVariant> binds;
    buildWhere(sql, binds, patientId, keyword, dept, dateFromIso, dateToIso);

    QSqlQuery q(db_);
    q.prepare(sql);
    for (int i=0;i<binds.size();++i) q.addBindValue(binds.at(i));
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}

QString Database::listOrdersAsJson(const QString &patientId,
                                   const QString &keyword,
                                   const QString &dept,
                                   const QString &dateFromIso,
                                   const QString &dateToIso,
                                   int page,
                                   int pageSize) {
    if (page < 1) page = 1;
    if (pageSize < 1) pageSize = 10;
    int offset = (page - 1) * pageSize;

    QString sql = "SELECT id, patient_id, doctor_id, dept, content, created_at FROM daorders";
    QList<QVariant> binds;
    buildWhere(sql, binds, patientId, keyword, dept, dateFromIso, dateToIso);
    sql += " ORDER BY id DESC LIMIT ? OFFSET ?";
    QSqlQuery q(db_);
    q.prepare(sql);
    int i;
    for (i=0;i<binds.size();++i) q.addBindValue(binds.at(i));
    q.addBindValue(pageSize);
    q.addBindValue(offset);

    QJsonArray arr;
    if (q.exec()) {
        while (q.next()) {
            QJsonObject o;
            o.insert("id", q.value(0).toInt());
            o.insert("patient_id", q.value(1).toString());
            o.insert("doctor_id", q.value(2).toString());
            o.insert("dept", q.value(3).toString());
            o.insert("content", q.value(4).toString());
            o.insert("created_at", q.value(5).toString());
            arr.append(o);
        }
    }
    QJsonDocument doc(arr);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

bool Database::updateOrder(int id, const QString &dept, const QString &content, QString *err) {
    QSqlQuery q(db_);
    q.prepare("UPDATE daorders SET dept=?, content=? WHERE id=?");
    q.addBindValue(dept);
    q.addBindValue(content);
    q.addBindValue(id);
    if (!q.exec()) {
        if (err) *err = q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}

bool Database::deleteOrder(int id, QString *err) {
    QSqlQuery q(db_);
    q.prepare("DELETE FROM daorders WHERE id=?");
    q.addBindValue(id);
    if (!q.exec()) {
        if (err) *err = q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}
