#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QJsonObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

enum class MessageParseError {
    UnknownError,
    InvalidRequestType
};

class ClientConnection : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnection(QWebSocket *sock, QObject *parent = 0);
    ~ClientConnection();

signals:

public slots:

private slots:
    void onTextMessageReceived(QString doc);

private:
    QWebSocket *socket;
    QJsonObject mostRecentObj;

    // debug methods
    QJsonDocument debugDemoSearch(QJsonObject obj);
};

#endif // CLIENTCONNECTION_H
