#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("问卷收集"); // 设置窗口标题
    //connect(ui->pushButton, &QPushButton::clicked, this, &Widget::on_submitButton_clicked);
}

Widget::~Widget()
{
    delete ui;
}

// 验证输入合法性
bool Widget::validateInput()
{
    // 验证姓名：不为空、长度合理、不包含特殊符号
    QString name = ui->lineEdit_name->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入姓名");
        ui->lineEdit_name->setFocus();
        return false;
    }
    if (name.length() > 20) {
        QMessageBox::warning(this, "输入错误", "请输入有效姓名");
        ui->lineEdit_name->setFocus();
        return false;
    }
    // 允许汉字、字母、空格和·（中间名分隔），禁止其他特殊符号
    QRegExp nameReg("^[a-zA-Z\u4e00-\u9fa5·\\s]+$");
    if (!nameReg.exactMatch(name)) {
        QMessageBox::warning(this, "输入错误", "姓名包含无效字符");
        ui->lineEdit_name->setFocus();
        return false;
    }

    // 验证年龄：必须为整数且​数字合理
    bool isNumber;
    int age = ui->lineEdit_age->text().toInt(&isNumber);
    if (ui->lineEdit_age->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "输入错误", "年龄不能为空");
        ui->lineEdit_age->setFocus();
        return false;
    }
    if (!isNumber) {
        QMessageBox::warning(this, "输入错误", "年龄必须是整数");
        ui->lineEdit_age->setFocus();
        return false;
    }
    if (age < 1 || age > 150) {
        QMessageBox::warning(this, "输入错误", "请输入有效年龄");
        ui->lineEdit_age->setFocus();
        return false;
    }

    // 验证性别：限制为合法值
    QString gender = ui->lineEdit_gender->text().trimmed();
    if (gender.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入性别");
        ui->lineEdit_gender->setFocus();
        return false;
    }
    QStringList validGenders = {"男", "女", "男性","女性", "Male", "Female","male","female"};
    if (!validGenders.contains(gender)) {
        QMessageBox::warning(this, "输入错误", "请输入有效性别");
        ui->lineEdit_gender->setFocus();
        return false;
    }

    // 验证身高：正数、合理范围
    QString heightStr = ui->lineEdit_height->text().trimmed();
    if (heightStr.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "身高不能为空");
        ui->lineEdit_height->setFocus();
        return false;
    }
    double height = heightStr.toDouble(&isNumber);
    if (!isNumber) {
        QMessageBox::warning(this, "输入错误", "身高必须是数字");
        ui->lineEdit_height->setFocus();
        return false;
    }
    if (height <= 0) {
        QMessageBox::warning(this, "输入错误", "身高必须是正数");
        ui->lineEdit_height->setFocus();
        return false;
    }
    if (height < 50 || height > 250) {
        QMessageBox::warning(this, "输入错误", "请输入有效身高");
        ui->lineEdit_height->setFocus();
        return false;
    }

    // 验证体重：正数、合理范围
    QString weightStr = ui->lineEdit_weight->text().trimmed();
    if (weightStr.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "体重不能为空");
        ui->lineEdit_weight->setFocus();
        return false;
    }
    double weight = weightStr.toDouble(&isNumber);
    if (!isNumber) {
        QMessageBox::warning(this, "输入错误", "体重必须是数字");
        ui->lineEdit_weight->setFocus();
        return false;
    }
    if (weight <= 0) {
        QMessageBox::warning(this, "输入错误", "体重必须是正数");
        ui->lineEdit_weight->setFocus();
        return false;
    }
    if (weight < 1 || weight > 300) {
        QMessageBox::warning(this, "输入错误", "请输入有效体重");
        ui->lineEdit_weight->setFocus();
        return false;
    }

    // 计算BMI
    BMI = weight * 10000 / height / height;  // 身高单位转换为米（cm->m）
    // 重置score再计算（避免累计错误）
    score = 0;
    if (BMI <= 18.4) score--; //偏瘦
    else if (BMI < 24) score++; //正常
    else if (BMI < 28) score--; //偏胖
    else if (BMI < 32) score -= 2; //肥胖
    else score -= 3; //重度肥胖

    // 验证手机号：11位数字且符合号段规则
    QString phone = ui->lineEdit_phonenum->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "手机号不能为空");
        ui->lineEdit_phonenum->setFocus();
        return false;
    }
    // 正则：1开头，第二位3-9，后9位数字（符合国内手机号规则）
    QRegExp phoneReg("^1[3-9]\\d{9}$");
    if (!phoneReg.exactMatch(phone)) {
        QMessageBox::warning(this, "输入错误", "请输入有效的11位手机号");
        ui->lineEdit_phonenum->setFocus();
        return false;
    }


    return true;
}

// 收集复选框选中项
QString Widget::collectSelectedOptions()
{
    QString selectedOptions;

    // 正面健康习惯（加分项）
    if (ui->checkBox_regdiet->isChecked()) {
        score++;
        selectedOptions += "规律饮食 ";
    }
    if (ui->checkBox_baldiet->isChecked()) {
        score++;
        selectedOptions += "均衡饮食 ";
    }
    if (ui->checkBox_regexe->isChecked()) {
        score++;
        selectedOptions += "规律运动 ";
    }
    if (ui->checkBox_enoslp->isChecked()) {
        score++;
        selectedOptions += "充足睡眠 ";
    }

    // 负面健康习惯（减分项）
    if (ui->checkBox_smodrk->isChecked()) {
        score--;
        selectedOptions += "吸烟饮酒 ";
    }
    if (ui->checkBox_strdep->isChecked()) {
        score--;
        selectedOptions += "压力抑郁 ";
    }
    if (ui->checkBox_chrdis->isChecked()) {
        score--;
        selectedOptions += "慢性疾病 ";
    }

    // 如果没有选择任何选项，返回默认值
    if (selectedOptions.isEmpty()) {
        selectedOptions = "无";
    }

    return selectedOptions.trimmed();
}

// 提交按钮点击事件处理
void Widget::on_pushButton_clicked()
{
    // 先验证输入
    if(!Widget::validateInput()) return;

    // 收集复选框选项并计算生活方式评分
    QString lifestyleChoices = collectSelectedOptions();

    // 收集所有数据
    QString name = ui->lineEdit_name->text().trimmed();
    QString age = ui->lineEdit_age->text().trimmed();
    QString gender = ui->lineEdit_gender->text().trimmed();
    QString height = ui->lineEdit_height->text().trimmed();
    QString weight = ui->lineEdit_weight->text().trimmed();
    QString phone = ui->lineEdit_phonenum->text().trimmed();

    // 计算BMI值用于显示
    double bmiValue = weight.toDouble() * 10000 / height.toDouble() / height.toDouble();

    // 根据BMI值获取身体状态描述
    QString bmiStatus;
    if (bmiValue <= 18.4) {
        bmiStatus = "偏瘦";
    } else if (bmiValue < 24) {
        bmiStatus = "正常";
    } else if (bmiValue < 28) {
        bmiStatus = "偏胖";
    } else if (bmiValue < 32) {
        bmiStatus = "肥胖";
    } else {
        bmiStatus = "重度肥胖";
    }

    // 格式化显示结果
    QString result = QString("提交成功！\n\n"
                            "姓名: %1\n"
                            "年龄: %2\n"
                            "性别: %3\n"
                            "身高: %4 cm\n"
                            "体重: %5 kg\n"
                            "手机号: %6\n"
                            "BMI指数: %7 (%8)\n"
                            "生活方式选择: %9\n"
                            "健康评分: %10")
                        .arg(name).arg(age).arg(gender)
                        .arg(height).arg(weight).arg(phone)
                        .arg(QString::number(bmiValue, 'f', 1))
                        .arg(bmiStatus)
                        .arg(lifestyleChoices)
                        .arg(score);

    // 显示提交结果
    QMessageBox::information(this, "提交成功", result);

    // 调试输出（可在控制台查看）
    qDebug() << "===== 问卷数据 =====";
    qDebug() << "姓名:" << name;
    qDebug() << "年龄:" << age;
    qDebug() << "性别:" << gender;
    qDebug() << "身高:" << height;
    qDebug() << "体重:" << weight;
    qDebug() << "手机号:" << phone;
    qDebug() << "BMI指数:" << bmiValue << "(" << bmiStatus << ")";
    qDebug() << "生活方式选择:" << lifestyleChoices;
    qDebug() << "健康评分:" << score;
    qDebug() << "====================";
}



