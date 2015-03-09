#include "httprequest.h"
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
    HttpRequest *req = new HttpRequest(server->nextPendingConnection());
    qDebug() << "Client has connected";

    handleGET(req->getSocket());
//    if (req->getRequestType() == GET) {
//        handleGET(req->getSocket());
//    } else {
//        qDebug() << "TODO: Not implemented";
//        req->getSocket()->disconnectFromHost();
//    }
}

void Server::handleGET(QTcpSocket* socket)
{
    QFile file("/home/sri/Downloads/sugar.mp3"); // got 99 problems but socket programming ain't one (yet)
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "File not openable, aborting...";
        exit(4);
    }

//    QByteArray data = socket->readAll();
//    qDebug() << data;
//    HttpRequest *req = new HttpRequest(&data);

//    while(1) {
//        if (socket->canReadLine()) {
//            qDebug() << socket->readLine();
//        }
//    }

    socket->write("HTTP/1.1 200 OK\r\n");
    socket->write("Content-Type: audio/mpeg\r\n");
//    socket->write("Accept-Ranges: bytes\r\n");
    socket->write("Content-Length: 9617355\r\n");
    socket->write("\r\n");
//    socket->write("Hello world!");

//     set up our byte array and file position pointer
    QByteArray block;
    qint64 pos = 0;

//      send file in 1024-byte chunks
    while(!file.atEnd()) {
        if(file.seek(pos) && (socket->state() != QAbstractSocket::UnconnectedState)) {
            qDebug() << "Sending bytes " << pos << " to " << pos + 1024;
            block = file.read(1024);
            socket->write(block);
            socket->waitForBytesWritten();
            pos += 1024;
            qDebug() << "Wrote bytes successfully";
        } else {
            qWarning() << "Everything has gone wrong, disconnecting.";
            break;
        }
    }

//    cleanup
    socket->disconnectFromHost();
    file.close();
}

Server::~Server()
{
    server->deleteLater();
}
