#include "ui_theme.h"
#include <QApplication>
#include <QPalette>
#include <QHeaderView>

namespace UiTheme {

static QString kStyle =
"QWidget{ background:#f5f6f8; color:#1f2329; font-family:'Microsoft YaHei','PingFang SC','Segoe UI','Arial'; font-size:16px; }"
"QLabel[hint='true']{ color:#8a9099; }"
"QLineEdit, QPlainTextEdit, QComboBox, QDateEdit, QSpinBox{ background:#ffffff; border:1px solid #e6e8eb; border-radius:10px; padding:8px 10px; }"
"QLineEdit:focus, QPlainTextEdit:focus, QComboBox:focus, QDateEdit:focus, QSpinBox:focus{ border:1px solid #2e7dff; box-shadow:0 0 0 2px rgba(46,125,255,0.15); }"
"QPushButton{ background:#2e7dff; color:#ffffff; border:none; border-radius:12px; padding:10px 18px; font-weight:600; }"
"QPushButton:hover{ background:#3b86ff; }"
"QPushButton:pressed{ background:#2a74f0; }"
"QPushButton:disabled{ background:#d7dbe1; color:#9096a3; }"
"QPushButton#secondary{ background:#eef2f8; color:#1f2329; }"
"QPushButton#secondary:hover{ background:#e6ecf6; }"
"QPushButton#ghost{ background:transparent; color:#2e7dff; border:1px solid rgba(46,125,255,0.35); }"
"QPushButton#ghost:hover{ background:rgba(46,125,255,0.06); }"
"QPushButton#danger{ background:#ff4d4f; }"
"QPushButton#danger:hover{ background:#ff6466; }"
"#appBar{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; padding:12px 16px; }"
"#sideBar{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; }"
"QGroupBox{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; margin-top:16px; }"
"QGroupBox::title{ subcontrol-origin: margin; left:12px; top:-10px; padding:0 8px; color:#1f2329; background:transparent; font-weight:600; }"
"QHeaderView::section{ background:#f3f5f8; padding:10px; border:none; border-bottom:1px solid #e6e8eb; font-weight:700; }"
"QTableWidget{ background:#ffffff; border:1px solid #e6e8eb; border-radius:12px; }"
"QTableWidget QTableCornerButton::section{ background:#f3f5f8; border:none; }"
"#avatar{ background:#dbe6ff; color:#245edb; border-radius:16px; padding:8px 12px; font-weight:700; }"
"#badge{ background:#ffefe0; color:#b85400; border-radius:9px; padding:3px 8px; font-weight:600; }";

void apply(QWidget *w) {
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Window, QColor("#f5f6f8"));
    pal.setColor(QPalette::Base,   QColor("#ffffff"));
    pal.setColor(QPalette::Text,   QColor("#1f2329"));
    pal.setColor(QPalette::Button, QColor("#2e7dff"));
    QApplication::setPalette(pal);
    w->setStyleSheet(kStyle);
}

QString cardStyle() {
    return "QFrame{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; }";
}

void styleTable(QTableWidget *table) {
    table->setAlternatingRowColors(true);
    table->setStyleSheet(
        "QTableWidget{ alternate-background-color:#fafbfc; }"
        "QTableWidget::item{ padding:8px; }"
        "QTableWidget::item:selected{ background:#e8f0ff; color:#1f2329; }"
    );
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(40);
    table->horizontalHeader()->setMinimumSectionSize(80);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

QString appBarStyle() { return "#appBar{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; padding:12px 16px; }"; }
QString sideBarStyle() { return "#sideBar{ background:#ffffff; border:1px solid #e6e8eb; border-radius:14px; }"; }

}
