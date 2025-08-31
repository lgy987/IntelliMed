QT       += core gui sql printsupport widgets

CONFIG += c++11

# 确保项目兼容Qt5，禁用Qt6特有功能
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050900    # 禁用Qt 5.9之前的已弃用API

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    database.cpp \
    e_invoice.cpp \
    main.cpp \
    medpay.cpp \
    paymenthistory.cpp \
    unpaid.cpp

HEADERS += \
    database.h \
    e_invoice.h \
    medpay.h \
    paymenthistory.h \
    unpaid.h

FORMS += \
    e_invoice.ui \
    medpay.ui \
    paymenthistory.ui \
    unpaid.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
