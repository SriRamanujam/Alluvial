#include "mediahandler.h"


/*!
 * \brief Provides a clean interface to the various modules involved
 * in finding music. Has interfaces to retrieve a byte array representing a media
 * file given a unique song hash and to search the various media sources for music.
 * \param parent
 */
MediaHandler::MediaHandler(QObject *parent) : QObject(parent)
{
    searchQueue = new QQueue<SearchResult*>();
    completedSearches = new QMap<QString, SearchResult*>();
    /// Demo code, make it prettier and/or functional later.
    spotify = new QtLibSpotifyHandler();
    soundcloud = new SCHandler();
    db = new queryhandler();
    dbSongs = new songHandler();
    crypto = new SimpleCrypt(Q_UINT64_C(0xb8828ee3d4532131));

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
 * \brief Take in a unique song hash corresponding to a media file
 * and return a QByteArray of that media file. Return an empty QByteArray when the result
 * is invalid, as well as emitting an error signal.
 * \param hash the unique song hash.
 * \return the media file.
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
        return dbSongs->getSong(split.at(1).toInt());
//    } else if (type == "spotify") {
//        return spotify->getMedia(split.at(1));
    } else {
        /// error handling passed up through here
        return QByteArray();
    }
}

/*!
 * \brief Dispatch search to all media source handlers and manages their results.
 *
 * This method first checks the completedSearches map for already-executed queries
 * matching the passed-in query string, and sends those results on if the query is
 * already present in the cache.
 * In the case that the query is new, this method instantiates a new SearchResult
 * object, connects it to all the necessary signals, and then calls each media
 * source's search function. The SearchResult object will handle all data collection,
 * formatting, and marshaling.
 * Finally, this function calls processQueue(), just in case there is an object
 * waiting to be popped off the queue.
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
        QJsonObject *res = completedSearches->value(query)->getSearchResults();
        emit searchResultComplete(*res); // will this work?
        return;
    }
    /// we first create the object and enqueue it for processing.
    SearchResult *search = new SearchResult(query);

    /// we now hook up all the needed signals to this object to ensure success
    connect(search, &SearchResult::searchProcessingComplete,
            this, &MediaHandler::processQueue);
//    connect(spotify, &QtLibSpotifyHandler::onSearchComplete,
//            search, &SearchResult::onSpotifySearchComplete);
    connect(db, &queryhandler::onSearchComplete,
            search, &SearchResult::onDbSearchComplete);
    connect(soundcloud, &SCHandler::onSearchComplete,
            search, &SearchResult::onSoundcloudSearchComplete);
    searchQueue->enqueue(search);

    /// execute the searches
    db->getResults(query);
//    spotify->search(query);
    soundcloud->search(query);

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
 * Due to the constraint placed upon us that searches are returned in the order
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
        QJsonObject *res = head->getSearchResults();
        completedSearches->insert(head->query, head);
        emit searchResultComplete(*res);
    } else {
        return;
    }
}
