#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include <QObject>
#include "JSONhandler.h"

/*!
 * \brief The queryhandler class handles queries from the client and
 * returns resuls to them
 */
class queryhandler : public QObject
{
    Q_OBJECT
public:
    queryhandler(QObject *parent = 0);
    ~queryhandler();
    QJsonArray getResults(QString query);

signals:
    void onSearchComplete(QJsonArray *obj);
};

#endif // QUERYHANDLER_H
