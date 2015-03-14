#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:

public slots:

private slots:
    void handleResponse();
    void onTextMessageReceived(QString doc);
    void onTextFrameReceived(QString doc, bool last);
private:
    QWebSocketServer *server;
    void initServer();
};

#endif // SERVER_H
