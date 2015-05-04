#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "databasehandler.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "sstream"


/*!
 * \brief The JSONHandler class handles creating the JSON results to be
 * sent to the client.
 */
class JSONHandler : public QObject
{
public:
    JSONHandler();
    ~JSONHandler();
    QJsonArray *generateResults(QString query);
};

#endif // JSONHandler_H
