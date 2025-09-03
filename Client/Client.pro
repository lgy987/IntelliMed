QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Include paths
INCLUDEPATH += include \
               include/doctor \
               include/patient \
               include/network \
               include/doctoradvice \
               include/HealthAssess \
               include/Cases


# Source files
SOURCES += \
    src/main.cpp \
    src/doctor/doctorhomepage.cpp \
    src/doctor/doctorloginform.cpp \
    src/doctor/doctorpersonalinfoform.cpp \
    src/doctor/doctorsession.cpp \
    src/doctor/doctorsignupform.cpp \
    src/doctor/doctorsessionform.cpp \
    src/patient/homepage.cpp \
    src/patient/loginform.cpp \
    src/patient/personalinfoform.cpp \
    src/patient/session.cpp \
    src/patient/signupform.cpp \
    src/patient/sessionform.cpp \
    src/patient/message.cpp \
    src/network/networkmanager.cpp \
    DoctorAdvice/patientwindow.cpp \
    DoctorAdvice/doctorwindow.cpp \
    DoctorAdvice/medlink.cpp \
    DoctorAdvice/orderdetaildialog.cpp \
    DoctorAdvice/ui_theme.cpp \
    HealthAssess/NetClient.cpp \
    HealthAssess/HealthAssess.cpp \
    HealthAssess/DataViewer.cpp \
    Cases/CaseNetworkManager.cpp \
    Cases/PatientView.cpp \
    Cases/DoctorView.cpp \
    Cases/CaseEditDialog.cpp


# Headers
HEADERS += \
    include/doctor/doctorhomepage.h \
    include/doctor/doctorloginform.h \
    include/doctor/doctorpersonalinfoform.h \
    include/doctor/doctorsession.h \
    include/doctor/doctorsignupform.h \
    include/doctor/doctorsessionform.h \
    include/patient/homepage.h \
    include/patient/loginform.h \
    include/patient/personalinfoform.h \
    include/patient/session.h \
    include/patient/sessionform.h \
    include/patient/signupform.h \
    include/patient/message.h \
    include/network/networkmanager.h \
    DoctorAdvice/patientwindow.h \
    DoctorAdvice/doctorwindow.h \
    DoctorAdvice/medlink.h \
    DoctorAdvice/orderdetaildialog.h \
    DoctorAdvice/ui_theme.h \
    HealthAssess/HealthAssess.h \
    HealthAssess/NetClient.h \
    HealthAssess/DataViewer.h \
    Cases/CaseNetworkManager.h \
    Cases/PatientView.h \
    Cases/DoctorView.h \
    Cases/CaseEditDialog.h

# Forms
FORMS += \
    forms/doctorhomepage.ui \
    forms/doctorloginform.ui \
    forms/doctorpersonalinfoform.ui \
    forms/doctorsignupform.ui \
    forms/homepage.ui \
    forms/loginform.ui \
    forms/personalinfoform.ui \
    forms/signupform.ui \
    forms/sessionform.ui \
    forms/doctorsessionform.ui \
    forms/message.ui \
    HealthAssess/HealthAssess.ui \
    Cases/PatientView.ui \
    Cases/DoctorView.ui \
    Cases/CaseEditDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
