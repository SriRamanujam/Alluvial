#include "queryhandler.h"
#include <QJsonDocument>

queryhandler::queryhandler(QObject *parent)
{

}

queryhandler::~queryhandler()
{

}
/*!
 * \brief gets the result QJsonArray
 *
 * \param query query in a QString
 * \return QJsonArray result
 */
QJsonArray queryhandler::getResults(QString query)
{
    QJsonArray *result;
    JSONHandler *mediaObject = new JSONHandler;

    result = mediaObject->generateResults(query);
    QByteArray guy = QJsonDocument(*result).toJson();
    qDebug() << "queryhandler has object" << QString(guy);
    emit onSearchComplete(result);
}

