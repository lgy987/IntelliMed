QT = core sql network

CONFIG += c++11 cmdline

SOURCES += \
        ai.cpp \
        dadb.cpp \
        doctoradviceserver.cpp \
        main.cpp \
        server.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ai.h \
    dadb.h \
    doctoradviceserver.h \
    server.h
