#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QNetworkReply>
#include <QBuffer>
#include <QNetworkAccessManager>

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
    void requestFinished(QNetworkReply* reply);
private:
    QMediaPlayer *player;
    QNetworkAccessManager *manager;
    void play();
    void initiateDownload();
    qint64 counter;
};

#endif // CLIENT_H
