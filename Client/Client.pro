QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    doctorhomepage.cpp \
    doctorloginform.cpp \
    doctorpersonalinfoform.cpp \
    doctorsession.cpp \
    doctorsignupform.cpp \
    homepage.cpp \
    main.cpp \
    loginform.cpp \
    networkmanager.cpp \
    personalinfoform.cpp \
    session.cpp \
    signupform.cpp

HEADERS += \
    doctorhomepage.h \
    doctorloginform.h \
    doctorpersonalinfoform.h \
    doctorsession.h \
    doctorsignupform.h \
    homepage.h \
    loginform.h \
    networkmanager.h \
    personalinfoform.h \
    session.h \
    signupform.h

FORMS += \
    doctorhomepage.ui \
    doctorloginform.ui \
    doctorpersonalinfoform.ui \
    doctorsignupform.ui \
    homepage.ui \
    loginform.ui \
    personalinfoform.ui \
    signupform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
