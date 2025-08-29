#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked(); // 提交按钮点击事件

private:
    Ui::Widget *ui;
    bool validateInput(); // 验证输入合法性
    QString collectSelectedOptions(); // 收集复选框选中项
    double BMI;
    int score=0;
};
#endif // WIDGET_H
