#include "mediahandler.h"

#include <QFile>

/*!
 * \brief This class provides a clean interface to the various modules involved
 * in finding music
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
    crypto = new SimpleCrypt(Q_UINT64_C(0x0x451823708829d4ce));

    // hook up our signals and our slots
}

MediaHandler::~MediaHandler()
{

}

/*!
 * \brief This method takes in a hash
 * \param hash
 * \return
 */
QByteArray MediaHandler::getMediaFromHash(QString hash)
{
    QString decrypt_string = crypto->decryptToString(hash);
    QStringList split = decrypt_string.split(": ");
    QString type = split.at(0);

    if (type == "spotify") {
        return spotify->getSong(split.at(1));
    } else if (type == "soundcloud") {
        return soundcloud->getSong(split.at(1));
    } else if (type == "db") {
        return db->getSong(split.at(1));
    } else {
        // error handling passed up through here
    }
}

/*!
 * \brief This function returns a valid results object for a given search query.
 * \param query The search string
 * \return A QJsonObject representing the results of the query.
 */
QJsonObject MediaHandler::search(QString query)
{
    // first, we check all the completed searches to see if the search has
    // already been executed. If it has been, then we return those results
    // instead of re-executing the search.
    if (completedSearches->contains(query)) {
        emit searchResultComplete(completedSearches[query].getSearchResults()); // will this work?
        return;
    }
    // we first create the object and enqueue it for processing.
    SearchResult *search = new SearchResult(query);

    // we now hook up all the needed signals to this object to ensure its success
    connect(search, SIGNAL(searchProcessingComplete()), SLOT(processQueue()));
    connect(spotify, SIGNAL(onSearchComplete(QJsonArray)),
            search, SLOT(onSpotifySearchComplete(QJsonArray)));
    connect(db, SIGNAL(onSearchComplete(QJsonArray)),
            search, SLOT(onDbSearchComplete(QJsonArray)));
    connect(soundcloud, SIGNAL(onSearchComplete(QJsonArray)),
            search, SLOT(onSoundcloudSearchComplete(QJsonArray)));
    searchQueue->enqueue(search);

    // do something to make sure the thing goes
    processQueue();
}

/*!
 * \brief Processes the queue.
 *
 * This function drives the queue processing forward one tick, so to speak. It
 * retrieves the current head of the queue, checks to see if that result is
 * complete, and retrieves the search results if it's available.
 *
 * Due to the constraint placed upon us that searches are returend in the order
 * they are received, there is no need to implement any sort of parallel
 * processing. This makes life a lot easier at the expense of some speed on the
 * backend. Because of this, we use this function to trigger advances in the queue.
 *
 * This method is called both when a new item is added to the queue and as a slot
 * when a search result completes.
 */
void MediaHandler::processQueue()
{
    SearchResult *head;
    if (searchQueue->isEmpty()) {
        return;
    }

    head = searchQueue->head();
    if (head->SEARCH_COMPLETE) {
        head = searchQueue->dequeue();
        QJsonObject res = head->getSearchResults();
        completedSearches->insert(head->query, head);
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
