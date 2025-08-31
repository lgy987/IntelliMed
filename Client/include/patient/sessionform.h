#ifndef SESSIONFORM_H
#define SESSIONFORM_H

#include <QWidget>

namespace Ui {
class SessionForm;
}

class SessionForm : public QWidget
{
    Q_OBJECT

public:
    explicit SessionForm(QWidget *parent = nullptr);
    ~SessionForm();

private slots:
    void onSessionInfoReceived(const QJsonObject &reply);

signals:
    void startMessage(int partnerId);

private:
    Ui::SessionForm *ui;
    void updateSessionUI();
    int m_partnerId;
};

#endif // SESSIONFORM_H
