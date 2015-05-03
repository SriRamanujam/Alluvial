#include "searchresult.h"
#include <QJsonDocument> /// for testing
///TODO: Re-add spotify

/*!
 * \brief Creates new empty QJsonArrays for all the different result objects,
 * and instantiates a new instance of SimpleCrypt for creating hashes.
 * \param parent
 * \param query The search query this object encapsulates.
 */
SearchResult::SearchResult(QString query, QObject *parent) : QObject(parent)
{
    dbRes = new QJsonArray();
    scRes = new QJsonArray();
    spotifyRes = new QJsonArray();
    resultsList = QJsonArray();
    crypto = new SimpleCrypt(Q_UINT64_C(0xb8828ee3d4532131));

    this->query = query;
    qDebug() << "SearchResult object created for" << query;
}

/*!
 * \brief Callback for when local database searches complete.
 *
 * This function receives a QJsonArray that contains the full search results
 * from the local database's search query. It then sets the global dbRes pointer
 * so that the SearchResult object can keep track of these results after the callback
 * completes, and flags the DB_COMPLETE boolean as true.
 *
 * Finally, it checks if all the callbacks have fired, and if it has, constructs
 * the full result.
 * \param obj A pointer to the QJsonArray containing the database's search results.
 */
void SearchResult::onDbSearchComplete(QJsonArray *obj)
{
    qDebug() << "DB search has completed for query" << query;
    if (SEARCH_COMPLETE) {
        qDebug() << "search query already complete; db";
        return;
    }
    dbRes = obj;
    DB_COMPLETE = true;
//    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
//        constructFullResult();
//    }
    if (SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief Callback for when SoundCloud searches complete.
 *
 * This function receives a QJsonArray that contains the full search results
 * from SoundCloud's search query. It then sets the global scRes pointer
 * so that the SearchResult object can keep track of these results after the callback
 * completes, and flags the SOUNDCLOUD_COMPLETE boolean as true.
 *
 * Finally, it checks if all the callbacks have fired, and if it has, constructs
 * the full result.
 * \param obj A pointer to the QJsonArray containing the SoundCloud search results.
 */
void SearchResult::onSoundcloudSearchComplete(QJsonArray *obj)
{
    qDebug() << "Soundcloud search has completed for query" << query;
    if (SEARCH_COMPLETE) {
        qDebug() << "search query already complete; soundcloud";
        return;
    }
    scRes = obj;
    SOUNDCLOUD_COMPLETE = true;
    //    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
    //        constructFullResult();
    //    }
    if (SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief Callback for when Spotify searches complete.
 *
 * This function receives a QJsonArray that contains the full search results
 * from Spotify's search query. It then sets the global spotifyRes pointer
 * so that the SearchResult object can keep track of these results after the callback
 * completes, and flags the SPOTIFY_COMPLETE boolean as true.
 *
 * Finally, it checks if all the callbacks have fired, and if it has, constructs
 * the full result.
 * \param obj A pointer to the QJsonArray containing the Spotify search results.
 */
void SearchResult::onSpotifySearchComplete(QJsonArray *obj)
{
    qDebug() << "Spotify search has completed for query" << query;
    if (SEARCH_COMPLETE) {
        return;
    }
    spotifyRes = obj;
    SPOTIFY_COMPLETE = true;
    //    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
    //        constructFullResult();
    //    }
    if (SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief Appends the input array of json objects into the class's results list.
 *
 * This function takes in an input array of media objects, creates their unique
 * media hashes, and stores the new json object with the hash into the fullResults
 * object for later serialization.
 * \param arr the array of input objects
 * \param type The type of search result. used in generating the hash.
 */
void SearchResult::insertObjectsIntoResults(QJsonArray *arr, SearchResultType type)
{
    QString input;
    QString resType;

    switch(type) {
    case SearchResultType::DB: resType = "db: ";
        break;
    case SearchResultType::SoundCloud: resType = "soundcloud: ";
        break;
    case SearchResultType::Spotify: resType = "spotify: ";
        break;
    }

    for (int i = 0; i < arr->size() ; i++) {
        QJsonValue val = arr->at(i);
        QJsonObject obj1 = val.toObject();

        if (obj1["hash"].isString()) {
            input = obj1["hash"].toString();
        } else if (obj1["hash"].isDouble()) {
            input = QString::number(obj1["hash"].toDouble());
        } else {
            qWarning() << "Object has invalid hash";
            continue;
        }

        QString hash = crypto->encryptToString(resType + input);
        qDebug() << "hashed value is" << hash;
        obj1["hash"] = hash;
        qDebug() << "Inserting object" << QString(QJsonDocument(obj1).toJson());
        resultsList.append(obj1);
    }
}

/*!
 * \brief Return a pointer to the fully generated results object.
 *
 * Used when retrieving cached result.
 * \return
 */
QJsonObject *SearchResult::getSearchResults()
{
    return &fullResult; /// I wonder how memory management works here, probably have
                       /// to store the pointer. Potential static memory leak here,
                       /// check with Valgrind.
}

/*!
 * \brief Constructs the full JSON search result to be sent to the client.
 *
 * It inserts all the search results into the object's fullResults object, then constructs
 * the QJsonObject containing the full search result.
 *
 * Once it finishes all this, it sets the SEARCH_COMPLETE flag to true and emits
 * the searchProcessingComplete() signal for the ClientConnection object.
 */
void SearchResult::constructFullResult()
{
    int i;
    QString hash;

    /// we begin by concatenating the media objects into one big array.
    insertObjectsIntoResults(dbRes, SearchResultType::DB);
    insertObjectsIntoResults(scRes, SearchResultType::SoundCloud);
//    insertObjectsIntoResults(spotifyRes);

    /// at this point, we get to build the full object.
    fullResult = QJsonObject
    {
        {"query", query}
    };

    /// \todo Bolster this by splitting everything up into the appropriate
    /// categories.
    QJsonObject results = QJsonObject
    {
        {"number", (resultsList.size())},
    };

    results["results"] = resultsList;


    qDebug() << "full results are" << QJsonDocument(results).toJson();

    fullResult["results"] = results;


    /// at this point, we're done. So we set the SEARCH_COMPLETE flag to true
    /// and (maybe?) emit a signal indicating this to the parent object, so that
    /// it can remove this result from the queue.
    SEARCH_COMPLETE = true;
    emit searchProcessingComplete();
}

/*!
 * \brief Handles destruction of the object in a safe and secure way.
 */
SearchResult::~SearchResult()
{

}

