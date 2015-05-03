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

#include "clientconnection.h"
#include "activesockets.h"

/*!
 * \brief The Server class is the top-level server class.
 *
 * It starts up all the needed parameters, hooks up all the necessary signals, etc.
 * Right now it's hard-coded to have the name "Alluvial Server" and to start in
 * non-secure mode. This may change in the future.
 */
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
private:
    /*!
     * \brief Websocket server that manages connections with clients.
     */
    QWebSocketServer *server;
    void initServer(quint64 port);
    /*!
     * \brief ActiveSockets instance
     */
    ActiveSockets *sockets;
    /*!
     * \brief The media handler object instantiated by the server
     */
    MediaHandler *mediaHandler;
};

#endif // SERVER_H
