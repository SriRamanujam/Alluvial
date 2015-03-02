#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer();
    initServer();

    // hook up our signal and slot so new connections are automatically handled.
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

    QFile file("/home/sri/Downloads/09-99-Problems.mp3"); // got 99 problems but socket programming ain't one (yet)
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "File not openable, aborting...";
        exit(4);
    }

    // set up our byte array and file position pointer
    QByteArray block;
    qint64 pos = 0;

    // send file in 1024-byte chunks
    while(!file.atEnd()) {
        if(file.seek(pos)) {
            qDebug() << "Sending bytes " << pos << " to " << pos + 1024;
            block = file.read(1024);
            socket->write(block);
            socket->waitForBytesWritten();
            pos += 1024;
            qDebug() << "Wrote bytes successfully";
        } else {
            qWarning() << "Could not seek to specified file position";
        }
    }

    // cleanup
    socket->disconnectFromHost();
    file.close();
}

Server::~Server()
{
    server->deleteLater();
}



