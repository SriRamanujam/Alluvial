#ifndef SCHANDLER_H
#define SCHANDLER_H

#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFile>

#define SC_CLIENT_ID "ae7e59c9fcb720a8c86d0d4e00d0b48b"
#define SC_CLIENT_SECRET "e49e945b8da6f59e5602801f7f4187cb"
#define USER_AGENT "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36"
#define SC_TRACKS_URL "https://api.soundcloud.com/tracks/"

/*!
 * \brief The SCHandler class is used to access the Soundcloud APIs. Allows searching and
 *        requests for downloadable songs from Soundcloud.
 */

class SCHandler : public QObject
{
    Q_OBJECT

private:
    QJsonArray raw_results;


    /*!
     \brief Used by search() to make the call to the Soundcloud API using
            a QT GET request.

     \param key: A QString corresponding to the key used to define the query
                 Defaults to "title" if no key is passed
     \param value: A QString corresponding to the value used in the query
     \return an integer value denoting the number of search results found
             or -1 in the case of a failed query
    */
    int query(QString key, QString value);

    /*!
     \brief Used to format raw JSON from Soundcloud into the format required
            by the MediaHandler.

     \param initial: a QJsonValue from the QJsonArray returned by search()
     \return A QJsonValue containing the JSON formatted for the MediaHandler
    */
    QJsonValue format(QJsonValue initial);
public:

    /*!
     \brief Constructor used to access member functions
    */
    SCHandler(QObject *parent=0);
    ~SCHandler();

    /*!
     \brief Searches the Soundcloud database using a key and associated
            value. This pair corresponds to song metadata that is used
            to request song results from Soundcloud. The search results
            are returned are all downloadable (as set by Soundcloud) and
            returned as an array of JSON documents (QJsonArray), populating
            results.

     \param key: A QString corresponding to the key used to define the query
                 Defaults to "title" if no key is passed
     \param value: A QString corresponding to the value used in the query
     \return A QJsonArray containing media objects for all the "downloadable"
             songs corresponding to the search
    */
    QJsonArray search(QString value, QString key=QString("q"));

    /*!
     \brief Searches the Soundcloud database using a key and associated
            value. This pair corresponds to song metadata that is used
            to request song results from Soundcloud. The search results
            are returned are all downloadable (as set by Soundcloud) and
            returned as an array of JSON documents (QJsonArray), populating
            results.
     \param count: the desired number of search results to return
     \param key: A QString corresponding to the key used to define the query
                 Defaults to "title" if no key is passed
     \param value: A QString corresponding to the value used in the query
     \return A QJsonArray containing the first "count" results
    */
    QJsonArray search(int count, QString value, QString key=QString("q"));

    /*!
     \brief Requests a song from Soundcloud using the download_url
            supplied by a Soundcloud query(). The value associated
            with the download_url key provides access to an interstitial
            url used to access the downloadable file url. This file is
            downloaded to the specified directory or to the current directory
            if not specified.

     \param download_url: The QString value associated with the download_url found
                          in a Soundcloud query JSON object
     \param target: QString associated absolute or relative path (starting at the project directory)
                    to which to save the file
     \return an integer value denoting the size of the file or -1 for an unsuccessful request
    */
    QByteArray request_song(QString download_url);

signals:
    void onSearchComplete(QJsonArray *jarray);

};
#endif // SCHANDLER_H
