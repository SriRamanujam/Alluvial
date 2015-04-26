#include <QCoreApplication>
#include "client.h"
#include "testclientdebug.h"
#include "communicationhandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestClientDebug *debug = new TestClientDebug();

   return a.exec();
}
