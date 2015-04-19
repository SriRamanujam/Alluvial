#include "searchresult.h"

/*!
 * \brief This object handles the entire lifecycle of a search.
 * \param query
 */
SearchResult::SearchResult(QString query)
{
    dbRes = new QJsonArray();
    scRes = new QJsonArray();
    spotifyRes = new QJsonArray();

    this->query = query;
}

/*!
 * \brief SearchResult::onDbSearchComplete
 * \param obj
 */
void SearchResult::onDbSearchComplete(QJsonArray obj)
{
    if (SEARCH_COMPLETE) {
        return;
    }
    dbRes = obj;
    DB_COMPLETE = true;
    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief SearchResult::onSoundcloudSearchComplete
 * \param obj
 */
void SearchResult::onSoundcloudSearchComplete(QJsonArray obj)
{
    if (SEARCH_COMPLETE) {
        return;
    }
    scRes = obj;
    SOUNDCLOUD_COMPLETE = true;
    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief SearchResult::onSpotifySearchComplete
 * \param obj The completed array
 */
void SearchResult::onSpotifySearchComplete(QJsonArray obj)
{
    if (SEARCH_COMPLETE) {
        return;
    }
    spotifyRes = obj;
    SOUNDCLOUD_COMPLETE = true;
    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief SearchResult::insertObjectsIntoResults
 * \param arr
 */
void SearchResult::insertObjectsIntoResults(QJsonArray *arr)
{
    for (i = 0; i < arr.size() ; i++) {
        hash = crypto->encryptToString(arr.at(i).toObject()["hash"].toString());
        arr.at(i).toObject()["hash"] = hash;
        resultsList.append(arr.at(i));
    }
}

/*!
 * \brief SearchResult::getSearchResults
 * \return
 */
QJsonObject SearchResult::getSearchResults()
{
    return fullResult; // I wonder how memory management works here, probably have
                       // to store the pointer. Potential static memory leak here,
                       // check with Valgrind.
}

/*!
 * \brief SearchResult::constructFullResult
 */
void SearchResult::constructFullResult()
{
    int i;
    QString hash;
    QJsonArray resultsList = new QJsonArray();
    // we begin by concatenating the media objects into one big array.
    insertObjectsIntoResults(dbRes);
    insertObjectsIntoResults(scRes);
    insertObjectsIntoResults(spotifyRes);

    // at this point, we get to build the full object.
    fullResult = new QJsonObject
    {
        {"response_type", "search"},
        {"query", query}
    };

    QJsonObject results = new QJsonObject
    {
        {"number", resultsList.size()},
        {"results", resultsList}
    };

    fullResult["results"] = results;

    // at this point, we're done. So we set the SEARCH_COMPLETE flag to true
    // and (maybe?) emit a signal indicating this to the parent object, so that
    // it can remove this result from the queue.
    SEARCH_COMPLETE = true;
    emit searchProcessingComplete();
}

/*!
 * \brief SearchResult::~SearchResult
 */
SearchResult::~SearchResult()
{

}

