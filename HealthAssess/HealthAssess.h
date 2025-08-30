#ifndef HEALTHASSESS_H
#define HEALTHASSESS_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include "DatabaseManager.h"
#include "DataViewer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HealthAssess; }
QT_END_NAMESPACE

class HealthAssess : public QWidget
{
    Q_OBJECT

public:
    HealthAssess(QWidget *parent = nullptr);
    ~HealthAssess();

private slots:
    void on_pushButton_clicked();
    void on_viewDataButton_clicked();

private:
    Ui::HealthAssess *ui;
    bool validateInput();
    QString collectSelectedOptions();
    double BMI;
    int score=0;
    
    // 新增成员
    DatabaseManager *m_dbManager;
    DataViewer *m_dataViewer;
    
    void setupDatabase();
    void setupUI();
};
#endif // HEALTHASSESS_H


