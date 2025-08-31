#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QSqlRecord>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
{
    // 设置数据库文件路径（在用户文档目录下）
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(documentsPath);
    if (!dir.exists("HealthAssess")) {
        dir.mkdir("HealthAssess");
    }
    m_databasePath = documentsPath + "/HealthAssess/health_assess.db";
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::initializeDatabase()
{
    // 创建SQLite数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databasePath);
    
    if (!m_database.open()) {
        qDebug() << "数据库连接失败:" << m_database.lastError().text();
        m_isConnected = false;
        return false;
    }
    
    m_isConnected = true;
    qDebug() << "数据库连接成功:" << m_databasePath;
    
    // 创建数据表
    if (!createTables()) {
        qDebug() << "创建数据表失败";
        return false;
    }
    
    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;
    
    // 创建患者问卷数据表
    QString createTableSQL = 
        "CREATE TABLE IF NOT EXISTS patient_assessments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "age INTEGER NOT NULL,"
        "gender TEXT NOT NULL,"
        "height REAL NOT NULL,"
        "weight REAL NOT NULL,"
        "phone TEXT NOT NULL,"
        "bmi REAL NOT NULL,"
        "bmi_status TEXT NOT NULL,"
        "lifestyle_choices TEXT NOT NULL,"
        "health_score INTEGER NOT NULL,"
        "submission_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")";
    
    if (!query.exec(createTableSQL)) {
        qDebug() << "创建数据表失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "数据表创建成功";
    return true;
}

bool DatabaseManager::insertPatientData(const QString &name, int age, const QString &gender,
                                      double height, double weight, const QString &phone,
                                      double bmi, const QString &bmiStatus, 
                                      const QString &lifestyleChoices, int healthScore)
{
    if (!m_isConnected) {
        qDebug() << "数据库未连接";
        return false;
    }
    
    QSqlQuery query;
    query.prepare(
        "INSERT INTO patient_assessments "
        "(name, age, gender, height, weight, phone, bmi, bmi_status, lifestyle_choices, health_score) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(name);
    query.addBindValue(age);
    query.addBindValue(gender);
    query.addBindValue(height);
    query.addBindValue(weight);
    query.addBindValue(phone);
    query.addBindValue(bmi);
    query.addBindValue(bmiStatus);
    query.addBindValue(lifestyleChoices);
    query.addBindValue(healthScore);
    
    if (!query.exec()) {
        qDebug() << "插入数据失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "患者数据插入成功，ID:" << query.lastInsertId();
    return true;
}

QList<QMap<QString, QVariant>> DatabaseManager::getPatientData()
{
    QList<QMap<QString, QVariant>> patientList;
    
    if (!m_isConnected) {
        qDebug() << "数据库未连接";
        return patientList;
    }
    
    QSqlQuery query("SELECT * FROM patient_assessments ORDER BY submission_time DESC");
    
    while (query.next()) {
        QMap<QString, QVariant> patient;
        QSqlRecord record = query.record();
        
        for (int i = 0; i < record.count(); ++i) {
            patient[record.fieldName(i)] = record.value(i);
        }
        
        patientList.append(patient);
    }
    
    return patientList;
}

QMap<QString, QVariant> DatabaseManager::getPatientByName(const QString &name)
{
    QMap<QString, QVariant> patient;
    
    if (!m_isConnected) {
        qDebug() << "数据库未连接";
        return patient;
    }
    
    QSqlQuery query;
    query.prepare("SELECT * FROM patient_assessments WHERE name = ? ORDER BY submission_time DESC LIMIT 1");
    query.addBindValue(name);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            patient[record.fieldName(i)] = record.value(i);
        }
    }
    
    return patient;
}

QMap<QString, QVariant> DatabaseManager::getPatientByPhone(const QString &phone)
{
    QMap<QString, QVariant> patient;
    
    if (!m_isConnected) {
        qDebug() << "数据库未连接";
        return patient;
    }
    
    QSqlQuery query;
    query.prepare("SELECT * FROM patient_assessments WHERE phone = ? ORDER BY submission_time DESC LIMIT 1");
    query.addBindValue(phone);
    
    if (query.exec() && query.next()) {
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            patient[record.fieldName(i)] = record.value(i);
        }
    }
    
    return patient;
}

bool DatabaseManager::isConnected() const
{
    return m_isConnected;
}

void DatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        m_isConnected = false;
        qDebug() << "数据库连接已关闭";
    }
}
