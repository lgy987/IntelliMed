#include "HealthAssess.h"
#include <QRegularExpression>
#include "ui_HealthAssess.h"
#include<QMessageBox>
HealthAssess::HealthAssess(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HealthAssess)
    , m_dataViewer(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("健康自测问卷");

    setupUI();
}

HealthAssess::~HealthAssess()
{
    delete ui;
}

bool HealthAssess::validateInput()
{
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
    QRegularExpression nameReg("^[a-zA-Z\u4e00-\u9fa5·\\s]+$");
    if (!nameReg.match(name).hasMatch()) {
        QMessageBox::warning(this, "输入错误", "姓名包含无效字符");
        ui->lineEdit_name->setFocus();
        return false;
    }

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

    BMI = weight * 10000 / height / height;
    score = 0;
    if (BMI <= 18.4) score--;
    else if (BMI < 24) score++;
    else if (BMI < 28) score--;
    else if (BMI < 32) score -= 2;
    else score -= 3;

    QString phone = ui->lineEdit_phonenum->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "手机号不能为空");
        ui->lineEdit_phonenum->setFocus();
        return false;
    }
    QRegularExpression phoneReg("^1[3-9]\\d{9}$");
    if (!phoneReg.match(phone).hasMatch()) {
        QMessageBox::warning(this, "输入错误", "请输入有效的11位手机号");
        ui->lineEdit_phonenum->setFocus();
        return false;
    }

    return true;
}

QString HealthAssess::collectSelectedOptions()
{
    QString selectedOptions;
    if (ui->checkBox_regdiet->isChecked()) { score++; selectedOptions += "规律饮食 "; }
    if (ui->checkBox_baldiet->isChecked()) { score++; selectedOptions += "均衡饮食 "; }
    if (ui->checkBox_regexe->isChecked()) { score++; selectedOptions += "规律运动 "; }
    if (ui->checkBox_enoslp->isChecked()) { score++; selectedOptions += "充足睡眠 "; }
    if (ui->checkBox_smodrk->isChecked()) { score--; selectedOptions += "吸烟饮酒 "; }
    if (ui->checkBox_strdep->isChecked()) { score--; selectedOptions += "压力抑郁 "; }
    if (ui->checkBox_chrdis->isChecked()) { score--; selectedOptions += "慢性疾病 "; }
    if (selectedOptions.isEmpty()) { selectedOptions = "无"; }
    return selectedOptions.trimmed();
}

void HealthAssess::on_pushButton_clicked()
{
    if (!HealthAssess::validateInput()) return;

    QString lifestyleChoices = collectSelectedOptions();

    QString name   = ui->lineEdit_name->text().trimmed();
    int age        = ui->lineEdit_age->text().toInt();
    QString gender = ui->lineEdit_gender->text().trimmed();
    double height  = ui->lineEdit_height->text().toDouble();
    double weight  = ui->lineEdit_weight->text().toDouble();
    QString phone  = ui->lineEdit_phonenum->text().trimmed();

    double bmiValue = weight * 10000.0 / height / height;
    QString bmiStatus;
    if (bmiValue <= 18.4)      bmiStatus = "偏瘦";
    else if (bmiValue < 24.0)  bmiStatus = "正常";
    else if (bmiValue < 28.0)  bmiStatus = "偏胖";
    else if (bmiValue < 32.0)  bmiStatus = "肥胖";
    else                       bmiStatus = "严重肥胖";

    // 你的 score 计算逻辑应该已经在类里
    // 如果 score 是成员/或从 UI 勾选中计算，这里直接用
    int scoreVal = score;

    QJsonObject payload{
        {"name", name},
        {"age", age},
        {"gender", gender},
        {"height", height},
        {"weight", weight},
        {"phone", phone},
        {"bmi", bmiValue},
        {"bmi_status", bmiStatus},
        {"lifestyle_choices", lifestyleChoices},
        {"health_score", scoreVal}
    };

    NetClient& client = NetClient::instance();
    connect(&client, &NetClient::patientInserted, this,
            [this, name, age, gender, height, weight, phone, bmiValue, bmiStatus, lifestyleChoices, scoreVal](bool ok){
                QString result = QString("提交成功！\n\n"
                                         "姓名: %1\n年龄: %2\n性别: %3\n身高: %4 cm\n体重: %5 kg\n手机号: %6\n"
                                         "BMI指数: %7 (%8)\n生活方式选择: %9\n健康评分: %10\n\n数据保存状态: %11")
                                     .arg(name).arg(age).arg(gender)
                                     .arg(height).arg(weight).arg(phone)
                                     .arg(QString::number(bmiValue, 'f', 1)).arg(bmiStatus)
                                     .arg(lifestyleChoices).arg(scoreVal)
                                     .arg(ok ? "已保存到后端数据库" : "保存失败");

                QMessageBox::information(this, "提交结果", result);

                // 清空表单/重置状态逻辑
                // ...
            });
    client.insertPatient(payload);
}



// 新增的UI设置函数
void HealthAssess::setupUI()
{
    // 连接UI文件中的查看数据按钮
    connect(ui->viewDataButton, &QPushButton::clicked, this, &HealthAssess::on_viewDataButton_clicked);
}

// 新增的查看数据槽函数
void HealthAssess::on_viewDataButton_clicked()
{
    if (!m_dataViewer) {
        m_dataViewer = new DataViewer(&NetClient::instance());
    }
    
    m_dataViewer->show();
    m_dataViewer->raise();
    m_dataViewer->activateWindow();
}


