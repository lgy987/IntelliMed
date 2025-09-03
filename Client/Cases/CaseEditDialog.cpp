#include "CaseEditDialog.h"
#include "ui_CaseEditDialog.h"
#include <QDebug>

CaseEditDialog::CaseEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaseEditDialog)
{
    ui->setupUi(this);
    setWindowTitle("编辑病例");

    // 连接确定和取消按钮
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

CaseEditDialog::~CaseEditDialog()
{
    delete ui;
}

void CaseEditDialog::setCaseData(const QJsonObject &caseData)
{
    m_caseData = caseData;

    // 填充表单字段
    ui->idEdit->setText(QString::number(caseData.value("id").toInt()));
    ui->patientNameEdit->setText(caseData.value("patient_name").toString());
    ui->patientIdEdit->setText(caseData.value("patient_id").toString());
    ui->virusTypeEdit->setText(caseData.value("virus_type").toString());
    ui->diagnosisDateEdit->setText(caseData.value("diagnosis_date").toString());
    ui->symptomsEdit->setPlainText(caseData.value("symptoms").toString());
    ui->treatmentEdit->setPlainText(caseData.value("treatment").toString());
    ui->doctorIdEdit->setText(caseData.value("doctor_id").toString());
    ui->doctorNameEdit->setText(caseData.value("doctor_name").toString());
    ui->departmentEdit->setText(caseData.value("department").toString());
}

QJsonObject CaseEditDialog::getCaseData() const
{
    QJsonObject caseData;
    caseData.insert("id", ui->idEdit->text().toInt());
    caseData.insert("patient_name", ui->patientNameEdit->text());
    caseData.insert("patient_id", ui->patientIdEdit->text());
    caseData.insert("virus_type", ui->virusTypeEdit->text());
    caseData.insert("diagnosis_date", ui->diagnosisDateEdit->text());
    caseData.insert("symptoms", ui->symptomsEdit->toPlainText());
    caseData.insert("treatment", ui->treatmentEdit->toPlainText());
    caseData.insert("doctor_id", ui->doctorIdEdit->text());
    caseData.insert("doctor_name", ui->doctorNameEdit->text());
    caseData.insert("department", ui->departmentEdit->text());

    return caseData;
}
