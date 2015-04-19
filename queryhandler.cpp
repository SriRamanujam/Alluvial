#include "queryhandler.h"

queryhandler::queryhandler()
{

}

queryhandler::~queryhandler()
{

}

QByteArray queryhandler::getSong(QString pk)
{
    // convert to integer
    songHandler *song = new songHandler();
    return song->getSong(pk.toInt());
}

void queryhandler::search(QString query)
{
    JSONHandler *json = new JSONHandler();
    emit onSearchComplete(json->generateResults(query));
}




