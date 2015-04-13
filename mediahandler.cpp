#include "mediahandler.h"

/*!
 * \brief MediaHandler::MediaHandler
 * \param parent
 */
MediaHandler::MediaHandler(QObject *parent) : QObject(parent)
{
    // Demo code, make it prettier and/or functional later.
//    spotify = new SpotifyHandler();
//    soundcloud = new SoundCloudHandler();
//    db = new DatabaseHandler();

    // hook up our signals and our slots

}

MediaHandler::~MediaHandler()
{

}

/*!
 * \brief MediaHandler::search
 * \param query
 * \return
 */
QJsonObject MediaHandler::search(QString query)
{

}

/*!
 * \brief MediaHandler::levenshtein_distance
 * \param s1
 * \param s2
 * \return
 */
unsigned int MediaHandler::levenshtein_distance(QString s1, QString s2)
{
    int len1 = s1.size(), len2 = s2.size();
    QList<unsigned int> col(), prevCol();

    for (unsigned int i = 0; i < len2 + 1; i++) {
        prevCol[i] = i;
    }
    for (unsigned int j = 0; j < len1; j++) {
        col[0] = j + 1;
        for (unsigned int k = 0; k < len2; k++) {
            col[k+1] = std::min({ prevCol.at(k+1) + 1, col.at(k) + 1, prevCol.at(k) + (s1.at(j)==s2.at(k)? 0 : 1)});
        }
        col.swap(prevCol);
    }
    return prevCol[len2];
}
