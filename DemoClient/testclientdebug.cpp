#include "testclientdebug.h"

TestClientDebug::TestClientDebug(QObject *parent) : QObject(parent)
{
    handler = new CommunicationHandler("127.0.0.1");

    handler->sendAuthRequest("password");
    handler->sendSearchRequest("Taylor Swift");
    handler->sendMediaRequest("aosenthuaoecrixaebrxasobeuhs");
}

TestClientDebug::~TestClientDebug()
{

}
