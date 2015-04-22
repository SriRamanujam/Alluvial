#include "queryhandler.h"

queryhandler::queryhandler(QObject *parent)
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
    QJsonArray res = json->generateResults(query);
    emit onSearchComplete(&res);
}




