#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QSettings>

class Session
{
public:
    static Session& instance();

    QString username() const;
    QString userId() const;
    QString token() const;

    void setUser(const QString& username, const QString& userId, const QString& token);
    void clear();

    void enablePersistence(bool enable); // new

private:
    Session();
    ~Session();
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    void save();
    void load();

    QString m_username;
    QString m_userId;
    QString m_token;
    bool m_persistent = false; // controls disk saving
};

#endif // SESSION_H
