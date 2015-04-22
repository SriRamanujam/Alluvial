#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include "JSONhandler.h"
#include "songhandler.h"


class queryhandler : public QObject
{
    Q_OBJECT
public:
    queryhandler(QObject *parent = 0);
    ~queryhandler();

    QByteArray getSong(QString pk);
    void search(QString query);

signals:
    void onSearchComplete(QJsonArray *arr);
};

#endif // QUERYHANDLER_H
