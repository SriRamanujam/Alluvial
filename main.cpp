#include <QApplication>
#include <QQmlApplicationEngine>

#include "mediahandler.h"
#include "qtlibspotifyhandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MediaHandler *handler = new MediaHandler();
    QtLibSpotifyHandler *spotify = new QtLibSpotifyHandler("mybutt", "yourbutt");

    return app.exec();
}
