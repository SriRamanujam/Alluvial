#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QObject>
#include <QDebug>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>

#include "simplecrypt.h"

/*!
 * \brief The SearchResultType enum denotes what media source a particular
 * set of search results belong to.
 */
enum class SearchResultType {
    DB = 0,         /*!< Local database */
    SoundCloud = 1, /*!< Soundcloud */
    Spotify = 2     /*!< Spotify */
};

/*!
 * \brief The SearchResult class handles the entire lifecycle of a search.
 *
 * This object manages dispatching searches, receiving search results, constructing
 * search results, and storing them in its internal cache for later retrieval.
 */
class SearchResult : public QObject
{
    Q_OBJECT
public:
    explicit SearchResult(QString query, QObject *parent = 0);
    ~SearchResult();
    bool SEARCH_COMPLETE = false;

    /*!
     * \brief The query string associated with the object.
     */
    QString query;

    QJsonObject* getSearchResults();
signals:
    /*!
     * \brief Emitted when the SearchResult object has completely finished
     * processing the search result, and is ready to be returned and dequeued.
     */
    void searchProcessingComplete();

private:
    /*!
     * \brief a flag indicating the status of the spotify query
     */
    bool SPOTIFY_COMPLETE = false;
    /*!
     * \brief flag indicating the status of soundcloud query
     */
    bool SOUNDCLOUD_COMPLETE = false;
    /*!
     * \brief flag indicating the status of local database query
     */
    bool DB_COMPLETE = false;

    /*!
     * \brief The cryptography object that creates hashes.
     */
    SimpleCrypt *crypto;

    /*!
     * \brief Pointer to the database results
     */
    QJsonArray *dbRes;
    /*!
     * \brief Pointer to the soundcloud results
     */
    QJsonArray *scRes;
    /*!
     * \brief pointer to the spotify results
     */
    QJsonArray *spotifyRes;

    /*!
     * \brief Full JSON result of the search
     */
    QJsonObject fullResult;

    /*!
     * \brief the array of JSON results
     */
    QJsonArray resultsList;

    void constructFullResult();
    void insertObjectsIntoResults(QJsonArray *arr, SearchResultType type);

public slots:
    void onSpotifySearchComplete(QJsonArray *obj);
    void onSoundcloudSearchComplete(QJsonArray *obj);
    void onDbSearchComplete(QJsonArray *obj);
};

#endif // SEARCHRESULT_H
