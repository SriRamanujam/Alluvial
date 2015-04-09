#include "testclientdebug.h"

TestClientDebug::TestClientDebug(QObject *parent) : QObject(parent)
{
    QWebSocket *socket = new QWebSocket(); // doesn't have to go anywhere or do anything
    handler = new CommunicationHandler(socket);

    handler->sendAuthRequest("password");
    handler->sendSearchRequest("Taylor Swift");
    handler->sendMediaRequest("aosenthuaoecrixaebrxasobeuhs");

}

TestClientDebug::~TestClientDebug()
{

}

