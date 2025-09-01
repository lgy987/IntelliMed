#ifndef MOODRECORDER_H
#define MOODRECORDER_H

#include <QObject>

class MoodRecorder : public QObject
{
    Q_OBJECT
public:
    explicit MoodRecorder(QObject *parent = nullptr);

signals:
};

#endif // MOODRECORDER_H
