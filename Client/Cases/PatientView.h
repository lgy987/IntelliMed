#ifndef PATIENTVIEW_H
#define PATIENTVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include "CaseNetworkManager.h"

namespace Ui {
class PatientView;
}

class PatientView : public QWidget
{
    Q_OBJECT

public:
    explicit PatientView(QWidget *parent = nullptr);
    ~PatientView();

public slots:
    void handleDataReceived(const QJsonObject &data);

private slots:
    void onSearchButtonClicked();

private:
    Ui::PatientView *ui;
    CaseNetworkManager *m_networkManager = &CaseNetworkManager::instance();
    QStandardItemModel *m_model;

    void setupTable();
};

#endif // PATIENTVIEW_H
