#include "mediahandler.h"


/*!
 * \brief This class provides a clean interface to the various modules involved
 * in finding music
 * \param parent
 */
MediaHandler::MediaHandler(QObject *parent) : QObject(parent)
{
    searchQueue = new QQueue<SearchResult*>();
    completedSearches = new QMap<QString, SearchResult*>();
    /// Demo code, make it prettier and/or functional later.
    spotify = new QtLibSpotifyHandler("mybutt", "yourbutt");
    soundcloud = new SCHandler();
    db = new queryhandler();
    crypto = new SimpleCrypt(Q_UINT64_C(0x451823708829d4ce));

    /// hook up our signals and our slots
    qDebug() << "SHIT WE RUNNIN NOW FAM";
}

void MediaHandler::retrievePlaylist(QString name)
{
//    emit getPlaylistComplete(db->getPlaylist(name));
}

void MediaHandler::savePlaylist(QJsonObject playlist)
{
//    db->save(playlist);
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

    /// TODO: Re-add spotify
    if (type == "soundcloud") {
        return soundcloud->request_song(split.at(1));
    } else if (type == "db") {
        return db->getSong(split.at(1));
    } else {
        /// error handling passed up through here
        return QByteArray();
    }
}

/*!
 * \brief This function returns a valid results object for a given search query.
 * \param query The search string
 * \return A QJsonObject representing the results of the query.
 */
void MediaHandler::search(QString query)
{
    /// first, we check all the completed searches to see if the search has
    /// already been executed. If it has been, then we return those results
    /// instead of re-executing the search.
    if (completedSearches->contains(query)) {
        qDebug() << "We found a search result already cached";
        QJsonObject res = completedSearches->value(query)->getSearchResults();
        emit searchResultComplete(res); // will this work?
        return;
    }
    /// we first create the object and enqueue it for processing.
    SearchResult *search = new SearchResult(query);

    /// we now hook up all the needed signals to this object to ensure success
    connect(search, &SearchResult::searchProcessingComplete,
            this, &MediaHandler::processQueue);
//    connect(spotify, SIGNAL(onSearchComplete(QJsonArray)),
//            search, SLOT(onSpotifySearchComplete(QJsonArray)));
    connect(db, &queryhandler::onSearchComplete,
            search, &SearchResult::onDbSearchComplete);
    connect(soundcloud, &SCHandler::onSearchComplete,
            search, &SearchResult::onSoundcloudSearchComplete);
    searchQueue->enqueue(search);

    /// execute the searches
    db->search(query);
//    spotify->search(query);
    soundcloud->query(query);

    /// do something to make sure the thing goes
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
 * backend. Because of this, we use this function to trigger advances in the
 * queue.
 *
 * This method is called both when a new item is added to the queue and as a
 * slot when a search result completes.
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
