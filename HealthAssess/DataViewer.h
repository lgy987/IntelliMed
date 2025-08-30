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
#include "DatabaseManager.h"

class DataViewer : public QWidget
{
    Q_OBJECT

public:
    explicit DataViewer(DatabaseManager *dbManager, QWidget *parent = nullptr);

private slots:
    void refreshData();
    void searchData();
    void exportData();
    void clearSearch();

private:
    void setupUI();
    void loadData();
    void setupTable();
    
    DatabaseManager *m_dbManager;
    QTableWidget *m_tableWidget;
    QLineEdit *m_searchLineEdit;
    QComboBox *m_searchFieldCombo;
    QPushButton *m_searchButton;
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    QPushButton *m_clearButton;
};

#endif // DATAVIEWER_H
