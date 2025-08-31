#include "database.h"
#include <QDir>
#include <QStandardPaths>

Database::Database(QObject *parent)
    : QObject(parent)
    , m_connected(false)
{
}

Database::~Database()
{
    closeDatabase();
}

bool Database::connectDatabase()
{
    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    
    // 创建数据库连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dataPath + "/medical_expenses.db");
    
    if (!m_db.open()) {
        qDebug() << "数据库连接失败:" << m_db.lastError().text();
        return false;
    }
    
    m_connected = true;
    qDebug() << "数据库连接成功:" << m_db.databaseName();
    
    // 创建表
    if (!createTables()) {
        qDebug() << "表创建失败";
        return false;
    }
    
    return true;
}

void Database::closeDatabase()
{
    if (m_connected) {
        m_db.close();
        m_connected = false;
        qDebug() << "数据库连接已关闭";
    }
}

bool Database::isConnected() const
{
    return m_connected;
}

bool Database::createTables()
{
    QSqlQuery query;
    
    // 创建医疗费用表
    if (!createMedicalExpensesTable()) {
        return false;
    }
    
    // 插入示例数据
    if (!insertSampleData()) {
        qDebug() << "示例数据插入失败";
    }
    
    return true;
}

bool Database::createMedicalExpensesTable()
{
    QSqlQuery query;
    
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS medical_expenses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            item_name VARCHAR(100) NOT NULL,
            unit_price DECIMAL(10,2) NOT NULL,
            quantity INTEGER NOT NULL,
            amount DECIMAL(10,2) NOT NULL,
            department VARCHAR(100) NOT NULL,
            charge_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            payment_method VARCHAR(50) NOT NULL,
            payment_status VARCHAR(50) DEFAULT '待支付'
        )
    )";
    
    if (!query.exec(createTableSQL)) {
        qDebug() << "创建医疗费用表失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "医疗费用表创建成功";
    return true;
}

bool Database::insertSampleData()
{
    QSqlQuery query;
    
    // 检查是否已有数据
    query.exec("SELECT COUNT(*) FROM medical_expenses");
    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "示例数据已存在，跳过插入";
        return true;
    }
    
    // 插入示例数据
    QStringList itemNames = {"挂号费", "检查费", "药品费", "床位费", "手术费", "护理费"};
    QStringList departments = {"门诊部", "放射科", "药房", "住院部", "手术室", "护理部"};
    QStringList paymentMethods = {"医保支付", "自费支付", "混合支付"};
    QStringList paymentStatuses = {"待支付", "已支付", "已退款"};
    
    for (int i = 0; i < itemNames.size(); ++i) {
        double unitPrice = 50.0 + i * 25.0;  // 不同的单价
        int quantity = 1 + (i % 3);           // 不同的数量
        double amount = unitPrice * quantity;
        QString paymentMethod = paymentMethods[i % paymentMethods.size()];
        QString paymentStatus = paymentStatuses[i % paymentStatuses.size()];
        
        if (!insertMedicalExpense(itemNames[i], unitPrice, quantity, amount, 
                                departments[i], paymentMethod, paymentStatus)) {
            qDebug() << "插入示例数据失败:" << itemNames[i];
            return false;
        }
    }
    
    qDebug() << "示例数据插入成功";
    return true;
}

bool Database::insertMedicalExpense(const QString &itemName, double unitPrice, int quantity, 
                                   double amount, const QString &department, 
                                   const QString &paymentMethod, const QString &paymentStatus)
{
    QSqlQuery query;
    
    query.prepare(R"(
        INSERT INTO medical_expenses 
        (item_name, unit_price, quantity, amount, department, payment_method, payment_status)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(itemName);
    query.addBindValue(unitPrice);
    query.addBindValue(quantity);
    query.addBindValue(amount);
    query.addBindValue(department);
    query.addBindValue(paymentMethod);
    query.addBindValue(paymentStatus);
    
    if (!query.exec()) {
        qDebug() << "插入医疗费用记录失败:" << query.lastError().text();
        return false;
    }
    
    // 发射数据变化信号，通知其他界面刷新数据
    emit dataChanged();
    
    return true;
}

bool Database::updatePaymentStatus(int id, const QString &status)
{
    QSqlQuery query;
    
    query.prepare("UPDATE medical_expenses SET payment_status = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "更新支付状态失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::deleteMedicalExpense(int id)
{
    QSqlQuery query;
    
    query.prepare("DELETE FROM medical_expenses WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qDebug() << "删除医疗费用记录失败:" << query.lastError().text();
        return false;
    }
    
    // 发射数据变化信号，通知其他界面刷新数据
    emit dataChanged();
    
    return true;
}

QSqlTableModel* Database::getUnpaidExpenses()
{
    QSqlTableModel *model = new QSqlTableModel(this, m_db);
    model->setTable("medical_expenses");
    model->setFilter("payment_status = '待支付'");
    model->setSort(6, Qt::DescendingOrder); // 按收费时间降序
    model->select();
    
    return model;
}

QSqlTableModel* Database::getPaymentHistory()
{
    QSqlTableModel *model = new QSqlTableModel(this, m_db);
    model->setTable("medical_expenses");
    model->setFilter("payment_status = '已支付'");
    model->setSort(6, Qt::DescendingOrder); // 按收费时间降序
    model->select();
    
    return model;
}

QSqlTableModel* Database::getInvoiceList()
{
    QSqlTableModel *model = new QSqlTableModel(this, m_db);
    model->setTable("medical_expenses");
    model->setSort(6, Qt::DescendingOrder); // 按收费时间降序
    model->select();
    
    return model;
}

double Database::getTotalUnpaidAmount()
{
    QSqlQuery query;
    query.exec("SELECT SUM(amount) FROM medical_expenses WHERE payment_status = '待支付'");
    
    if (query.next()) {
        return query.value(0).toDouble();
    }
    
    return 0.0;
}

int Database::getUnpaidCount()
{
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM medical_expenses WHERE payment_status = '待支付'");
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

double Database::getTotalPaidAmount()
{
    QSqlQuery query;
    query.exec("SELECT SUM(amount) FROM medical_expenses WHERE payment_status = '已支付'");
    
    if (query.next()) {
        return query.value(0).toDouble();
    }
    
    return 0.0;
}

int Database::getPaidCount()
{
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM medical_expenses WHERE payment_status = '已支付'");
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

bool Database::dropTables()
{
    QSqlQuery query;
    
    if (!query.exec("DROP TABLE IF EXISTS medical_expenses")) {
        qDebug() << "删除表失败:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "表删除成功";
    return true;
}
