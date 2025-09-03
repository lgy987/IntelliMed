#ifndef DATAVIEWER_H
#define DATAVIEWER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include "NetClient.h"

class DataViewer : public QWidget
{
    Q_OBJECT
public:
    explicit DataViewer(NetClient* client, QWidget *parent = nullptr);

private slots:
    void loadData();
    void searchData();
    void exportData();
    void clearSearch();

private:
    void setupUI();
    void setupTable();
    void handleAllPatients(const QJsonArray &arr);

    NetClient* m_client;             // 改为网络客户端
    QTableWidget *m_tableWidget;
    QLineEdit *m_searchLineEdit;
    QComboBox *m_searchFieldCombo;
    QPushButton *m_searchButton;
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    QPushButton *m_clearButton;
};

#endif // DATAVIEWER_H
