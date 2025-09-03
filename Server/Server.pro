QT = core sql network

CONFIG += c++11 cmdline

SOURCES += \
        dadb.cpp \
        doctoradviceserver.cpp \
        hadb.cpp \
        healthassessserver.cpp \
        CaseDatabase.cpp \
        CaseServer.cpp \
        main.cpp \
        server.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    dadb.h \
    doctoradviceserver.h \
    hadb.h \
    healthassessserver.h \
    CaseDatabase.h \
    CaseServer.h \
    server.h
