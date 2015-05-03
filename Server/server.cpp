/**
  @file
*/

#include "server.h"

/*!
 * \brief Instatiates QWebSocketServer, ActiveSockets, and MediaHandler singletons,
 * calls initServer() to set up the server, and sets up signals and slots for
 * the class to respond to new and closed connections.
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
 * \brief Sets up server and starts it listening on all interfaces on the
 * specified port. By default the server is set to listen on port 8900.
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
 * \brief Slot called when a new connection is made. This method receives the
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
