#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioBuffer>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();

signals:

public slots:
private slots:
    void printStatus(qint64 pos);
private:
    QMediaPlayer *player;
};

#endif // CLIENT_H
