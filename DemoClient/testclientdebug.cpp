#include "testclientdebug.h"

TestClientDebug::TestClientDebug(QObject *parent) : QObject(parent)
{
    handler = new CommunicationHandler("http://127.0.0.1:8900");

    connect(handler, SIGNAL(onAuthReceived(bool)),
            this, SLOT(authReceivedCallback(bool)));
    connect(handler, SIGNAL(onSearchReceived(QJsonObject)),
            this, SLOT(searchReceivedCallback(QJsonObject)));
}

TestClientDebug::~TestClientDebug()
{

}

void TestClientDebug::authReceivedCallback(bool auth)
{
    qDebug() << "Auth received on client, result was " << auth;
    handler->sendSearchRequest("Taylor Swift");
}

void TestClientDebug::searchReceivedCallback(QJsonObject obj)
{
    qDebug() << "Search result received:";
    qDebug() << QString(QJsonDocument(obj).toJson());
}
