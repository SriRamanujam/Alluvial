#include <QCoreApplication>
#include "client.h"
#include "testclientdebug.h"
#include "communicationhandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    Client *thing = new Client();
    TestClientDebug* debug = new TestClientDebug();

   return a.exec();
}
