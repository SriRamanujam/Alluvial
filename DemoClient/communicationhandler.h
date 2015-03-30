#ifndef COMMUNICATIONHANDLER_H
#define COMMUNICATIONHANDLER_H

#include <QJsonDocument>
#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

class CommunicationHandler : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationHandler(QObject *parent = 0);
    ~CommunicationHandler();

    //methods to do things
    void sendSearchRequest(QString req);
    void sendMediaRequest(QString hash);
    void sendAuthRequest(QString pass); // should we handle this as a part of init?

    void sendSettings();
    void sendPlaylist();

    void connectToServer(QString host);

signals:
    // when things happen, call these guys. Literally the Ghostbusters.
    QJsonDocument onPlaylistReceived(QString doc);
    QJsonDocument onSearchReceived(QJsonDocument doc);
    QByteArray onMediaReceived(QByteArray data);
    QJsonDocument onAuthReceived(QString doc);
    QJsonDocument onErrorReceived(QString doc);

    // error handling
    void emitError();
public slots:
    
private:
    QWebSocket *socket;

private slots:
    void handleBinaryMessage(QByteArray data);
    void handleTextMessage(QString doc);
    void reconnectToServer();
};

#endif // COMMUNICATIONHANDLER_H
