#include "songhandler.h"

songHandler::songHandler()
{

}

songHandler::~songHandler()
{

}

QByteArray songHandler::getSong(int PKID)
{
    DatabaseHandler *dub = new DatabaseHandler();
    bool DBOpen = dub->openDB();
    QString FP = dub->getSongFP(PKID);
    QFile file(FP);
    QByteArray song = file.readAll();
    return song;
}

