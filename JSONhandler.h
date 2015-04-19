#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "databasehandler.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>



class JSONHandler : public QObject
{
public:
    JSONHandler();
    ~JSONHandler();
    QJsonArray generateResults(QString query);

private:
    DatabaseHandler *dub;
};

#endif // JSONHandler_H
