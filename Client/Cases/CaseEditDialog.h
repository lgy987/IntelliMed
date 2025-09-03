#ifndef CASEEDITDIALOG_H
#define CASEEDITDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class CaseEditDialog;
}

class CaseEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaseEditDialog(QWidget *parent = nullptr);
    ~CaseEditDialog();

    void setCaseData(const QJsonObject &caseData);
    QJsonObject getCaseData() const;

private:
    Ui::CaseEditDialog *ui;
    QJsonObject m_caseData;
};

#endif // CASEEDITDIALOG_H
