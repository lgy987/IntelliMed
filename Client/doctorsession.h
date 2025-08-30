#ifndef DOCTORSESSION_H
#define DOCTORSESSION_H

#include <QString>
#include <QSettings>

class DoctorSession
{
public:
    static DoctorSession& instance();

    QString username() const;
    QString userId() const;
    QString token() const;

    void setUser(const QString& username, const QString& userId, const QString& token);
    void clear();

    void enablePersistence(bool enable); // new

private:
    DoctorSession();
    ~DoctorSession();
    DoctorSession(const DoctorSession&) = delete;
    DoctorSession& operator=(const DoctorSession&) = delete;

    void save();
    void load();

    QString m_username;
    QString m_userId;
    QString m_token;
    bool m_persistent = false; // controls disk saving
};

#endif // SESSION_H
