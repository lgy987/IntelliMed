#ifndef AI_H
#define AI_H

#include <QObject>

class Ai : public QObject
{
    Q_OBJECT
public:
    explicit Ai(QObject *parent = nullptr){};
    static void callAI(const QString &userMessage);

signals:
};

#endif // AI_H
