#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QWebSocket>
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
    void debugPrintError(QAbstractSocket::SocketError error);
    void debugSendData();
    void debugCloseClient();
private:
    QMediaPlayer *player;
    QNetworkAccessManager *manager;
    QWebSocket* socket;
    qint64 counter;
};

#endif // CLIENT_H
