#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QWebSocketServer(QStringLiteral("Alluvial Server"),
                                  QWebSocketServer::NonSecureMode, this);
    initServer();

    // hook up our signal and slot so new connections are automatically handled.
    connect(server, SIGNAL(newConnection()), this, SLOT(handleResponse()));
    connect(server, SIGNAL(closed()), this, SLOT(deleteLater()));
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
    qDebug() << "Client has connected";

    QWebSocket *socket = server->nextPendingConnection();
    connect(socket, SIGNAL(textMessageReceived(QString)),
            this, SLOT(onTextMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)),
            socket, SLOT(close()));
    connect(socket, SIGNAL(binaryFrameReceived(QByteArray,bool)),
            socket, SLOT(close()));
}

void Server::onTextMessageReceived(QString doc)
{
    qDebug("Received message from client");
    qDebug() << doc;

    QJsonDocument json = QJsonDocument::fromJson(doc.toUtf8());
    QJsonObject obj = json.object();

    qDebug() << obj["properties"];

    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        QString msg = "I have sent a reply back to you! Huzzah!";
//        client->sendTextMessage(msg);

        QFile file("/home/sri/Downloads/sugar.mp3");
        file.open(QIODevice::ReadOnly);
        QByteArray data = file.readAll();
        qDebug() << "Read file in, now sending";
        client->sendBinaryMessage(data);
    } else {
        qDebug() << "reverse socket not found";
    }

}

void Server::onTextFrameReceived(QString doc, bool last)
{

}

Server::~Server()
{
    server->deleteLater();
}
