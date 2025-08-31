#ifndef PERSONALINFOFORM_H
#define PERSONALINFOFORM_H

#include <QWidget>
#include <utility>

namespace Ui {
class PersonalInfoForm;
}

class PersonalInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInfoForm(QWidget *homepage, QWidget *parent = nullptr);
    ~PersonalInfoForm();
    // Static helper function to parse ID and return sex and age as strings
    static std::pair<QString, QString> parseIdStatic(const QString &id);

private slots:
    void onEditClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onPersonalInfoReceived(const QJsonObject &reply);

private:
    QWidget *homepage;
    Ui::PersonalInfoForm *ui;
    void setEditMode(bool enable);
    void parseIdNumber(const QString &id);
    void autoUpdateInfo(const QString &id);

};


#endif // PERSONALINFOFORM_H
