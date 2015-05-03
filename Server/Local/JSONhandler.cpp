#include "JSONhandler.h"

#include <QJsonDocument>


JSONHandler::JSONHandler()
{

}

JSONHandler::~JSONHandler()
{

}
/*!
 * \brief generates a QJsonArray of results for a search query
 *
 * Iterates through all results and turns them into transmitable data
 * \param query QString representing a search query
 * \return QJsonArray of results for a search query. Results are QJsonObjects called media objects.
 */
QJsonArray* JSONHandler::generateResults(QString query)
{
    QJsonObject test;
    QJsonArray *result = new QJsonArray();
    std::vector <MD> MDresult;
    DatabaseHandler *dub = new DatabaseHandler();
    bool DBOpen = dub->openDB();
    MDresult = dub->queryDB(query);
    int numOfMD = MDresult.size();

    //qDebug() << MDresult.at(0).PKID;

    for(int i = 0; i < numOfMD; i++){
        std::stringstream sm;
        std::stringstream ss;
        string stringLengthTemp;
        sm <<  MDresult.at(i).length_min;
        ss << MDresult.at(i).length_sec;
        int secs = MDresult.at(i).length_sec;
        if(secs < 10)
            stringLengthTemp = sm.str() + ":0" + ss.str();
        else {
            stringLengthTemp = sm.str() + ":" + ss.str();
        }
        QString qstrLength = QString::fromStdString(stringLengthTemp);
        qDebug() << qstrLength;
        QJsonObject media{
            {"hash", MDresult.at(i).PKID},
            {"order",""},
        };
        QJsonObject meta{
            {"title", MDresult.at(i).title},
            {"album", MDresult.at(i).album},
            {"artist", MDresult.at(i).artist},
            {"track_number", MDresult.at(i).track_num},
            {"length", qstrLength},
            {"genre", MDresult.at(i).genre}
        };
        media["metadata"] = meta;
        result->append(media);
    }
    return result;
}

