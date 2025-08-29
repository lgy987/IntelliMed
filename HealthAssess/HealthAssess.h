#ifndef HEALTHASSESS_H
#define HEALTHASSESS_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>

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

private:
    Ui::HealthAssess *ui;
    bool validateInput();
    QString collectSelectedOptions();
    double BMI;
    int score=0;
};
#endif // HEALTHASSESS_H


