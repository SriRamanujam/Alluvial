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
    explicit CommunicationHandler(QString url, QObject *parent = 0);
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
    void onPlaylistReceived(QJsonDocument doc);
    void onSearchReceived(QJsonObject doc);
    void onMediaReceived(QByteArray data);
    void onAuthReceived(bool success);
    void onErrorReceived(QString doc);

    // error handling. these are emitted when things go wrong.
    void playlistError(QString err);
    void searchError(QString err);
    void mediaError(QString err);
    void authError(QString err);
    void connectionLost();
public slots:
    
private:
    QWebSocket *socket;

    void handleSearchResponse(QJsonObject obj);
    void handleAuthResponse(QJsonObject obj);
    void handlePlaylistResponse(QJsonObject obj);

private slots:
    void handleBinaryMessage(QByteArray data);
    void handleTextMessage(QString doc);
    void reconnectToServer();
    void initiateAuthReq();
};

#endif // COMMUNICATIONHANDLER_H
