#include "doctorsession.h"

DoctorSession::DoctorSession() {
    load();
}

DoctorSession::~DoctorSession() {
    if (m_persistent) save();
}

DoctorSession& DoctorSession::instance() {
    static DoctorSession s;
    return s;
}

QString DoctorSession::username() const { return m_username; }
QString DoctorSession::userId() const { return m_userId; }
QString DoctorSession::token() const { return m_token; }

void DoctorSession::enablePersistence(bool enable) {
    m_persistent = enable;
}

void DoctorSession::setUser(const QString& username, const QString& userId, const QString& token) {
    m_username = username;
    m_userId = userId;
    m_token = token;

    if (m_persistent) save(); // save only if persistence is enabled
}

void DoctorSession::clear() {
    m_username.clear();
    m_userId.clear();
    m_token.clear();
    if (m_persistent) save();
}

void DoctorSession::save() {
    QSettings settings("MyCompany", "MyApp");
    settings.setValue("username", m_username);
    settings.setValue("userId", m_userId);
    settings.setValue("token", m_token);
}

void DoctorSession::load() {
    QSettings settings("MyCompany", "MyApp");
    m_username = settings.value("username").toString();
    m_userId = settings.value("userId").toString();
    m_token = settings.value("token").toString();
}
