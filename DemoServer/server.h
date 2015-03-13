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
    void debugPrintResults(QString doc);
private:
    QWebSocketServer *server;
    void initServer();
};

#endif // SERVER_H
