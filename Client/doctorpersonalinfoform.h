#ifndef DOCTORPERSONALINFOFORM_H
#define DOCTORPERSONALINFOFORM_H

#include <QWidget>

namespace Ui {
class DoctorPersonalInfoForm;
}

class DoctorPersonalInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorPersonalInfoForm(QWidget *homepage, QWidget *parent = nullptr);
    ~DoctorPersonalInfoForm();

private slots:
    void onEditClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onPersonalInfoReceived(const QJsonObject &reply);

private:
    QWidget *homepage;
    Ui::DoctorPersonalInfoForm *ui;
    void setEditMode(bool enable);
};


#endif // PERSONALINFOFORM_H
