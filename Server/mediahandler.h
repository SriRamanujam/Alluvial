#ifndef MEDIAHANDLER_H
#define MEDIAHANDLER_H
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QStringList>
#include <QMap>
#include <QFile>

#include "searchresult.h"
#include "simplecrypt.h"
#include "Local/queryhandler.h"
#include "Local/songhandler.h"
#include "Soundcloud/schandler.h"
#include "Spotify/qtlibspotifyhandler.h"

/*!
 * \brief The MediaHandler class provides a clean interface to the various modules involved
 * in finding music.
 *
 * It has interfaces to retrieve a byte array representing a media file given a
 * unique song hash and to search the various media sources for music. It also
 * provides different signals for asynchronous result handling by the client
 * connection.
 *
 * This class is instantiated as a singleton by the main Server class during startup,
 * and pointers to it are passed to each SearchResult object for access purposes.
 *
 * \todo Re-add Spotify`
 */
class MediaHandler : public QObject
{
    Q_OBJECT
public:
    explicit MediaHandler(QObject *parent = 0);
    ~MediaHandler();

    void search(QString query);
    QByteArray getMediaFromHash(QString hash);
    void retrievePlaylist(QString name);
    void savePlaylist(QJsonObject playlist);

    SimpleCrypt *crypto;

signals:
    /*!
     * \brief Emittted when the search result is completely processed
     * \param res The fully built result object
     */
    void searchResultComplete(QJsonObject res);
    /*!
     * \brief Emitted when the playlist object is completely received and processed
     * \param res The fully built playlist object
     */
    void getPlaylistComplete(QJsonObject res);


public slots:
private:
    /*!
     * \brief The Spotify interface object.
     */
    QtLibSpotifyHandler *spotify;
    /*!
     * \brief The Soundcloud interface object.
     */
    SCHandler *soundcloud;
    /*!
     * \brief The local database query object.
     */
    queryhandler *db;
    /*!
     * \brief The local database song retrieval object.
     */
    songHandler *dbSongs;
    /*!
     * \brief The queue of search results currently being processed.
     */
    QQueue<SearchResult*> *searchQueue;
    /*!
     * \brief A map of completed SearchResult objects, indexed by query.
     */
    QMap<QString, SearchResult*> *completedSearches;
    
//    unsigned int levenshtein_distance(QString s1, QString s2);

private slots:
    void processQueue();

};

#endif // MEDIAHANDLER_H
