#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // 初始化数据库连接
    bool initializeDatabase();
    
    // 创建数据表
    bool createTables();
    
    // 插入患者问卷数据
    bool insertPatientData(const QString &name, int age, const QString &gender,
                          double height, double weight, const QString &phone,
                          double bmi, const QString &bmiStatus, 
                          const QString &lifestyleChoices, int healthScore);
    
    // 查询患者数据
    QList<QMap<QString, QVariant>> getPatientData();
    
    // 根据姓名查询患者数据
    QMap<QString, QVariant> getPatientByName(const QString &name);
    
    // 根据手机号查询患者数据
    QMap<QString, QVariant> getPatientByPhone(const QString &phone);
    
    // 获取数据库连接状态
    bool isConnected() const;
    
    // 关闭数据库连接
    void closeDatabase();

private:
    QSqlDatabase m_database;
    bool m_isConnected;
    
    // 数据库文件路径
    QString m_databasePath;
};

#endif // DATABASEMANAGER_H
