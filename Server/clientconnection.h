#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QJsonObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

enum class MessageParseError {
    UnknownError = 0,
    InvalidRequestType = 1,
    ParseError = 2
};

class ClientConnection : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnection(QWebSocket *sock, QObject *parent = 0);
    ~ClientConnection();

signals:
    void authRequestProcessed();
    void searchReqParsed();
    void mediaReqParsed();

public slots:
    void onTextMessageReceived(QString doc);

private slots:


private:
    QWebSocket *socket;
    QJsonObject mostRecentObj;

    //parsing methods
    void _handleAuthenticationReq(QJsonObject req);
    void _handleSearchReq(QJsonObject req);
    void _handleMediaReq(QJsonObject req);

    //oooo nobody should see this
    void _handleSettingsReq(QJsonObject req);


    // debug methods
    QJsonDocument debugDemoSearch(QJsonObject obj);

    //build error message
    QJsonDocument buildErrorMsg(MessageParseError err);
};

#endif // CLIENTCONNECTION_H
