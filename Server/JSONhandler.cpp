#include "JSONhandler.h"
#include "sstream"


JSONHandler::JSONHandler()
{
    dub = new DatabaseHandler();
}

JSONHandler::~JSONHandler()
{

}

QJsonArray JSONHandler::generateResults(QString query)
{
    QJsonObject test;
    QJsonArray result;
    std::vector <MD> MDresult;
    bool DBOpen = dub->openDB();
    MDresult = dub->queryDB(query);
    int numOfMD = MDresult.size();

    //qDebug() << MDresult.at(0).PKID;

    for(int i = 0; i < numOfMD; i++) {
        std::stringstream sm;
        std::stringstream ss;
        sm <<  MDresult.at(i).length_min;
        ss <<  MDresult.at(i).length_sec;
        string stringLengthTemp = sm.str() + ":" + ss.str();
        QString qstrLength = QString::fromStdString(stringLengthTemp);
        QJsonObject media {
            {"hash", MDresult.at(i).PKID},
            {"order",""},
        };
        QJsonObject meta {
            {"title", MDresult.at(i).title},
            {"album", MDresult.at(i).album},
            {"artist", MDresult.at(i).artist},
            {"track_number", MDresult.at(i).track_num},
            {"length", qstrLength},
            {"genre", MDresult.at(i).genre}
        };
        media["metadata"] = meta;
        result.append(media);
    }
    //qDebug() << result;
    return result;
}

