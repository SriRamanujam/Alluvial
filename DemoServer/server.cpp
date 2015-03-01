#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer();
    initServer();

    // hook up our signal and slot so it's automatically handled.
    connect(server, SIGNAL(newConnection()), this, SLOT(handleResponse()));
}

void Server::initServer()
{
    if (!server->listen(QHostAddress::Any, 8900)) {
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

void Server::handleResponse()
{
    // hook up our socket such that it destroys itself when it's done sending
    // stuff.
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << "We're just writing a string to see if it worked.";

    socket->write(block);
    socket->disconnectFromHost();
}

Server::~Server()
{
    server->deleteLater();
}



