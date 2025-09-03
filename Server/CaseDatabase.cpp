#include "CaseDatabase.h"

CaseDatabase::CaseDatabase(QObject *parent) : QObject(parent)
{
    initDatabase();
}

CaseDatabase::~CaseDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool CaseDatabase::initDatabase()
{
    // 设置数据库路径 - 使用绝对路径确保可写
    QString dbPath = QDir::currentPath() + "/medical_cases.db";
    qDebug() << "Database path:" << dbPath;

    m_db = QSqlDatabase::addDatabase("QSQLITE", "Cases");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Error: Failed to open database -" << m_db.lastError().text();
        return false;
    }

    // 检查数据库是否可写
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);
    if (!query.exec("PRAGMA journal_mode=WAL")) {
        qDebug() << "Error setting journal mode:" << query.lastError().text();
    }

    // 创建表
    if (!createTables()) {
        qDebug() << "Error: Failed to create tables";
        return false;
    }

    // 插入示例数据
    if (!insertSampleData()) {
        qDebug() << "Error: Failed to insert sample data";
        return false;
    }

    qDebug() << "Database initialized successfully";
    return true;
}

bool CaseDatabase::createTables()
{
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);

    // 创建病例表
    QString createCaseTable = "CREATE TABLE IF NOT EXISTS cases ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "patient_name TEXT NOT NULL, "
                             "patient_id TEXT NOT NULL, "
                             "virus_type TEXT NOT NULL, "
                             "diagnosis_date TEXT NOT NULL, "
                             "symptoms TEXT, "
                             "treatment TEXT, "
                             "doctor_id TEXT NOT NULL, "
                             "doctor_name TEXT NOT NULL, "
                             "department TEXT NOT NULL)";

    if (!query.exec(createCaseTable)) {
        qDebug() << "Error creating cases table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool CaseDatabase::insertSampleData()
{
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);

    // 检查是否已有数据
    query.exec("SELECT COUNT(*) FROM cases");
    if (query.next() && query.value(0).toInt() > 0) {
        return true; // 已有数据，不需要插入
    }

    // 插入示例病例数据
    QString insertData = "INSERT INTO cases (patient_name, patient_id, virus_type, diagnosis_date, symptoms, treatment, doctor_id, doctor_name, department) VALUES "
                        "('张三', 'P1001', 'COVID-19', '2023-05-10', '发烧、咳嗽、呼吸困难', '药物治疗、休息', 'D2001', '李医生', '呼吸内科'),"
                        "('李四', 'P1002', '流感', '2023-05-12', '发烧、头痛、肌肉酸痛', '抗病毒药物、休息', 'D2002', '王医生', '内科'),"
                        "('王五', 'P1003', 'COVID-19', '2023-05-15', '轻微咳嗽、疲劳', '居家隔离、观察', 'D2001', '李医生', '呼吸内科'),"
                        "('赵六', 'P1004', '诺如病毒', '2023-05-18', '腹泻、呕吐、腹痛', '补液、对症治疗', 'D2003', '张医生', '消化内科')";

    if (!query.exec(insertData)) {
        qDebug() << "Error inserting sample data:" << query.lastError().text();
        return false;
    }

    return true;
}

QJsonArray CaseDatabase::queryCasesByVirus(const QString &virus, bool isDoctorView)
{
    QJsonArray result;
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);

    QString sql;
    if (isDoctorView) {
        // 医生视图：显示所有匹配的病例
        sql = "SELECT * FROM cases WHERE virus_type LIKE :virus";
        query.prepare(sql);
        query.bindValue(":virus", "%" + virus + "%");
    } else {
        // 患者视图：只显示基本信息
        sql = "SELECT patient_name, virus_type, diagnosis_date, doctor_name, department FROM cases WHERE virus_type LIKE :virus";
        query.prepare(sql);
        query.bindValue(":virus", "%" + virus + "%");
    }

    qDebug() << "执行SQL查询:" << sql;
    qDebug() << "参数virus:" << virus;

    if (!query.exec()) {
        qDebug() << "查询错误:" << query.lastError().text();
        return result;
    }

    int count = 0;
    while (query.next()) {
        count++;
        QJsonObject record;
        if (isDoctorView) {
            // 医生视图返回完整信息
            record.insert("id", query.value("id").toInt());
            record.insert("patient_name", query.value("patient_name").toString());
            record.insert("patient_id", query.value("patient_id").toString());
            record.insert("virus_type", query.value("virus_type").toString());
            record.insert("diagnosis_date", query.value("diagnosis_date").toString());
            record.insert("symptoms", query.value("symptoms").toString());
            record.insert("treatment", query.value("treatment").toString());
            record.insert("doctor_id", query.value("doctor_id").toString());
            record.insert("doctor_name", query.value("doctor_name").toString());
            record.insert("department", query.value("department").toString());
        } else {
            // 患者视图返回有限信息
            record.insert("patient_name", query.value("patient_name").toString());
            record.insert("virus_type", query.value("virus_type").toString());
            record.insert("diagnosis_date", query.value("diagnosis_date").toString());
            record.insert("doctor_name", query.value("doctor_name").toString());
            record.insert("department", query.value("department").toString());
        }
        result.append(record);
    }

    qDebug() << "找到" << count << "条匹配记录";
    return result;
}

QJsonArray CaseDatabase::queryAllCases()
{
    QJsonArray result;
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);
    query.exec("SELECT * FROM cases");

    while (query.next()) {
        QJsonObject record;
        record.insert("id", query.value("id").toInt());
        record.insert("patient_name", query.value("patient_name").toString());
        record.insert("patient_id", query.value("patient_id").toString());
        record.insert("virus_type", query.value("virus_type").toString());
        record.insert("diagnosis_date", query.value("diagnosis_date").toString());
        record.insert("symptoms", query.value("symptoms").toString());
        record.insert("treatment", query.value("treatment").toString());
        record.insert("doctor_id", query.value("doctor_id").toString());
        record.insert("doctor_name", query.value("doctor_name").toString());
        record.insert("department", query.value("department").toString());
        result.append(record);
    }

    return result;
}

// 在文件末尾添加updateCase方法
bool CaseDatabase::updateCase(const QJsonObject &caseData)
{
    QSqlDatabase usersDb = QSqlDatabase::database("Cases");
    QSqlQuery query(usersDb);

    QString updateSql = "UPDATE cases SET "
                       "patient_name = :patient_name, "
                       "patient_id = :patient_id, "
                       "virus_type = :virus_type, "
                       "diagnosis_date = :diagnosis_date, "
                       "symptoms = :symptoms, "
                       "treatment = :treatment, "
                       "doctor_id = :doctor_id, "
                       "doctor_name = :doctor_name, "
                       "department = :department "
                       "WHERE id = :id";

    query.prepare(updateSql);
    query.bindValue(":patient_name", caseData.value("patient_name").toString());
    query.bindValue(":patient_id", caseData.value("patient_id").toString());
    query.bindValue(":virus_type", caseData.value("virus_type").toString());
    query.bindValue(":diagnosis_date", caseData.value("diagnosis_date").toString());
    query.bindValue(":symptoms", caseData.value("symptoms").toString());
    query.bindValue(":treatment", caseData.value("treatment").toString());
    query.bindValue(":doctor_id", caseData.value("doctor_id").toString());
    query.bindValue(":doctor_name", caseData.value("doctor_name").toString());
    query.bindValue(":department", caseData.value("department").toString());
    query.bindValue(":id", caseData.value("id").toInt());

    if (!query.exec()) {
        qDebug() << "Error updating case:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
