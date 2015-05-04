#include "songhandler.h"

songHandler::songHandler()
{

}

songHandler::~songHandler()
{

}
/*!
 * \brief opens up the db, given a PKID, returns song as QBytearray
 * \param PKID number representing unique song in DB
 * \return returns song as QBytearray
 */
QByteArray songHandler::getSong(int PKID)
{
    DatabaseHandler *dub = new DatabaseHandler();
    bool DBOpen = dub->openDB();
    QString FP = dub->getSongFP(PKID);
    QFile file(FP);
    file.open(QIODevice::ReadOnly);
    QByteArray song = file.readAll();
    return song;
}

