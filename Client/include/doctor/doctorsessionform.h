#ifndef DOCTORSESSIONFORM_H
#define DOCTORSESSIONFORM_H

#include <QWidget>
#include <QJsonObject>
#include <QList>

namespace Ui {
class DoctorSessionForm;
}

class DoctorSessionForm : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorSessionForm(QWidget *parent = nullptr);
    ~DoctorSessionForm();

private slots:
    void onDoctorSessionReceived(const QJsonObject &reply);
    void showPreviousSession();
    void showNextSession();
    void endCurrentSession();
    void handleEndSessionResponse(const QJsonObject &reply);

private:
    void showSession(int index);
    void updateSessionUI();

private:
    Ui::DoctorSessionForm *ui;

    QList<QJsonObject> sessions;  // Stores all sessions of the doctor
    int currentIndex;             // Current session being displayed
};

#endif // DOCTORSESSIONFORM_H
