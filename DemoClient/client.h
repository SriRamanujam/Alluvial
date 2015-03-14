#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QFile>
#include <QMediaPlayer>
#include <QWebSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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
    void onTextMessageReceived(QString msg);
    void onBinaryMessageReceived(QByteArray data);
    void debugCloseClient();
private:
    QMediaPlayer *player;
    QNetworkAccessManager *manager;
    QWebSocket* socket;
    qint64 counter;
};

#endif // CLIENT_H
