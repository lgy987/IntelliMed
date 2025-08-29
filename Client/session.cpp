#include "session.h"

Session::Session() {
    load();
}

Session::~Session() {
    if (m_persistent) save();
}

Session& Session::instance() {
    static Session s;
    return s;
}

QString Session::username() const { return m_username; }
QString Session::userId() const { return m_userId; }
QString Session::token() const { return m_token; }

void Session::enablePersistence(bool enable) {
    m_persistent = enable;
}

void Session::setUser(const QString& username, const QString& userId, const QString& token) {
    m_username = username;
    m_userId = userId;
    m_token = token;

    if (m_persistent) save(); // save only if persistence is enabled
}

void Session::clear() {
    m_username.clear();
    m_userId.clear();
    m_token.clear();
    if (m_persistent) save();
}

void Session::save() {
    QSettings settings("MyCompany", "MyApp");
    settings.setValue("username", m_username);
    settings.setValue("userId", m_userId);
    settings.setValue("token", m_token);
}

void Session::load() {
    QSettings settings("MyCompany", "MyApp");
    m_username = settings.value("username").toString();
    m_userId = settings.value("userId").toString();
    m_token = settings.value("token").toString();
}
