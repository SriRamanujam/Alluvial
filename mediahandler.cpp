#include "mediahandler.h"

/*!
 * \brief This class provides a clean interface to the various modules involved
 * in finding music.
 * \param parent
 */
MediaHandler::MediaHandler(QObject *parent) : QObject(parent)
{
    searchQueue = new QQueue<SearchResult*>();
    completedSearches = new QMap<QString, SearchResult*>();
    // Demo code, make it prettier and/or functional later.
    spotify = new SpotifyHandler();
    soundcloud = new SoundCloudHandler();
    db = new DatabaseHandler();

    // hook up our signals and our slots
}

MediaHandler::~MediaHandler()
{

}

/*!
 * \brief This function returns a query string and returns a QJsonObject representing
 * a "results" object. For more information regarding what exactly a "results" object is,
 * consult JSON.md.
 * \param query
 * \return
 */
void MediaHandler::search(QString query)
{
    // we first create the object and enqueue it for processing.
    SearchResult *search = new SearchResult(query);
    searchQueue->enqueue(search);

    // do something to make sure the thing goes
    processQueue();
}

void MediaHandler::processQueue()
{
    SearchResult *head;
    if (searchQueue->isEmpty()) {
        return;
    }

    head = searchQueue->head();
    if (head->SEARCH_COMPLETE) {
        head = searchQueue->dequeue();
        QJsonObject res = head->getSearchResultArray();
        emit searchResultComplete(res);
    } else {
        return;
    }
}



/*!
 * \brief This function implements the Levenshtein distance algorithm for calculating
 * the degree of similarity between two strings. We use it to aid in determining whether
 * two files are the same piece of music.
 *
 * The higher the number, the less similar the two strings are. Two of the same string
 * passed in gives 0 as the result.
 * \param s1
 * \param s2 The strings to be compared
 * \return an unsigned int representing the degree of similarity between the two strings.
 * The higher the number, the less similar the two strings are.
 */
unsigned int MediaHandler::levenshtein_distance(QString s1, QString s2)
{
    const int len1 = s1.size(), len2 = s2.size();
    QVector<unsigned int> col(len2+1), prevCol(len2+1);
    int i, j;

    for (i = 0; i < prevCol.size(); i++) {
        prevCol[i] = i;
    }
    for (i = 0; i < len1; i++) {
        col[0] = i+1;
        for (j = 0; j < len2; j++) {
            col[j+1] = std::min((prevCol[1+j] + 1, col[j] + 1), prevCol[j] + (s1[i]==s2[j] ? 0 : 1));
        }
        col.swap(prevCol);
    }
    return prevCol[len2];
}

void MediaHandler::onSpotifySearchComplete(QJsonObject obj)
{

}

void MediaHandler::onSoundcloudSearchComplete(QJsonObject obj)
{

}

void MediaHandler::onDbSearchComplete(QJsonObject obj)
{

}
