#include "searchresult.h"

/*!
 * \brief SearchResult::SearchResult
 * \param query
 */
SearchResult::SearchResult(QString query)
{
    dbRes = new QJsonObject();
    scRes = new QJsonObject();
    spotifyRes = new QJsonObject();

    hasher = new QCryptographicHash(QCryptographicHash::Sha256);
}

/*!
 * \brief SearchResult::onDbSearchComplete
 * \param obj
 */
void SearchResult::onDbSearchComplete(QJsonObject obj)
{
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
void SearchResult::onSoundcloudSearchComplete(QJsonObject obj)
{
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
void SearchResult::onSpotifySearchComplete(QJsonObject obj)
{
    spotifyRes = obj;
    SOUNDCLOUD_COMPLETE = true;
    if (SPOTIFY_COMPLETE && SOUNDCLOUD_COMPLETE && DB_COMPLETE) {
        constructFullResult();
    }
}

/*!
 * \brief SearchResult::constructFullResult
 */
void SearchResult::constructFullResult()
{

}

SearchResult::~SearchResult()
{

}

