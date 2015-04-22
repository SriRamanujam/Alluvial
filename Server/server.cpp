/**
  @file
*/

#include "server.h"

/*!
 * \brief This class is the top-level server class. It starts up all the needed
 * parameters, hooks up all the necessary signals, etc. Right now it's hard-coded
 * to have the name "Alluvial Server" and to start in non-secure mode. This may
 * change in the future.
 *
 * \param parent
 */
Server::Server(QObject *parent) : QObject(parent)
{
    server = new QWebSocketServer(QStringLiteral("Alluvial Server"),
                                  QWebSocketServer::NonSecureMode, this);
    initServer(8900);
    sockets = new ActiveSockets();
    mediaHandler = new MediaHandler();

    // hook up our signal and slot so new connections are automatically handled.
    connect(server, SIGNAL(newConnection()), this, SLOT(handleResponse()));
    connect(server, SIGNAL(closed()), this, SLOT(deleteLater()));
}

/*!
 * \brief Sets up server, instantiates all singleton handlers, and starts
 * the server listening on all interfaces on the specified port. By default the
 * server is set to listen on port 8900.
 */
void Server::initServer(quint64 port)
{
    if (!server->listen(QHostAddress::Any, port)) {
        qWarning() << "Server not able to bind to port, aborting";
        exit(1);
    }

    QString ip;
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();

    // search for and use the first non-localhost IP address.
    for (int i = 0; i < ipList.size(); ++i) {
        if (ipList.at(i) != QHostAddress::LocalHost &&
                ipList.at(i).toIPv4Address()) {
            ip = ipList.at(i).toString();
            break;
        }
    }

    // If we didn't find anything, use localhost.
    if (ip.isEmpty()) {
        ip = QHostAddress(QHostAddress::LocalHost).toString();
    }

    qDebug() << "The server is up on " << ip << ":" << server->serverPort();
}

/*!
 * \brief Called when a new connection is made. This method receives the
 * new connection, creates a new ClientConnection object, and stores it in the
 * ActiveSockets instance associated with the class.
 */
void Server::handleResponse()
{
    qDebug() << "Client has connected";

    QWebSocket *socket = server->nextPendingConnection();
    ClientConnection* conn = new ClientConnection(socket, mediaHandler);
    sockets->addConnection(conn);
}
/*!
 * \brief Handles safe and secure deletion of all objects and state.
 */
Server::~Server()
{
    server->deleteLater();
}
