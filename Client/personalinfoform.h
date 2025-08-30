#ifndef PERSONALINFOFORM_H
#define PERSONALINFOFORM_H

#include <QWidget>

namespace Ui {
class PersonalInfoForm;
}

class PersonalInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInfoForm(QWidget *homepage, QWidget *parent = nullptr);
    ~PersonalInfoForm();

private slots:
    void onEditClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onPersonalInfoReceived(const QJsonObject &reply);

private:
    QWidget *homepage;
    Ui::PersonalInfoForm *ui;
    void setEditMode(bool enable);
};


#endif // PERSONALINFOFORM_H
