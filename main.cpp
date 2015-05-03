#include <QApplication>

#include "Server/server.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("AlluvialAlphaBuild");
    app.setOrganizationDomain("couponbug.com");
    app.setApplicationName("Alluvial");

    Server *server = new Server();

    int appInt = app.exec();
    //Code to exit on close of program

    return appInt;
}

