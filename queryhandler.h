#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include "JSONhandler.h"
#include "songhandler.h"


class queryhandler
{
public:
    queryhandler();
    ~queryhandler();

    QByteArray getSong(QString pk);
    void search(QString query);

signals:
    void onSearchComplete(QJsonArray arr);
};

#endif // QUERYHANDLER_H
