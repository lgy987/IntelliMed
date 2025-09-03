#ifndef DOCTORVIEW_H
#define DOCTORVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QPushButton>
#include <QToolBar>
#include <QTableView>
#include "CaseNetworkManager.h"

namespace Ui {
class DoctorView;
}

class DoctorView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorView(QWidget *parent = nullptr);
    ~DoctorView();

public slots:
    void handleDataReceived(const QJsonObject &data);

private slots:
    void onSearchButtonClicked();
    void onEditButtonClicked();
    void onTableDoubleClicked(const QModelIndex &index);
    void handleUpdateResponse(const QJsonObject &data);

private:
    Ui::DoctorView *ui;
    CaseNetworkManager *m_networkManager = &CaseNetworkManager::instance();
    QStandardItemModel *m_model;
    QPushButton *m_editButton;

    void setupTable();
    void setupToolbar();
    void editCase(int row);
};

#endif // DOCTORVIEW_H
