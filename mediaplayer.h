#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H
#include <QByteArray>
#include <QMediaPlayer>
#include <QObject>

class mediaPlayer:public QObject
{
    Q_OBJECT

public:
    mediaPlayer();
    ~mediaPlayer();
    void play(QByteArray);
    double getMediaDuration();
    int positionChanged();

public slots:
    void play();
    void pause();
    void skipTo(int);
    void playOrPause();
    void setVolume(int);
    void startRewind();
    void startFastForward();
    void resetPlaybackRate();
    void childPositionChanged(qint64);

signals:
    qint64 durationChanged();
    void positionChanged(QVariant);

private:
    QMediaPlayer *player;
};


#endif // MEDIAPLAYER_H
