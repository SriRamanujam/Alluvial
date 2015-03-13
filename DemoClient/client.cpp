#include "client.h"
#include <unistd.h>

Client::Client(QObject *parent) : QObject(parent)
{
    //hardcoding these for now since i'm lazy
    QString SERVER_HOST = "192.168.2.7";
    qint64 SERVER_PORT = 8900;
    counter = 0;

    qDebug() << "Starting data transfer";

    const QString data = QString("{'properties' : 'yurmum' }");
    socket = new QWebSocket();
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(debugPrintError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(debugSendData()));

    socket->open(QUrl("ws://192.168.2.7:8900"));

}

Client::~Client()
{
//    player->deleteLater();
}

void Client::debugPrintError(QAbstractSocket::SocketError error)
{
    qDebug() << "ERR: error is" << error;
}

void Client::debugSendData()
{
    const QString data = QString("{properties: yurmum}");
    qDebug() << "Connected to " << socket->requestUrl();
    quint64 data_sent = socket->sendTextMessage(data);
    qDebug() << "Sent" << data_sent << "bytes of data";
}

void Client::debugCloseClient()
{
    qDebug() << "Server terminated connection, closing";
}

