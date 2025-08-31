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

private:
    Ui::SessionForm *ui;
    void updateSessionUI();
};

#endif // SESSIONFORM_H
