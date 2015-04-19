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

class SCHandler
{

private:

public:
    QJsonObject last_search;
    QJsonArray raw_results;
    SCHandler();
    ~SCHandler();

    /*!
     \brief Queries the Soundcloud database using a key and associated
            value. This pair corresponds to song metadata that is used
            to request song results from Soundcloud. The search results
            are returned are all downloadable (as set by Soundcloud) and
            returned as an array of JSON documents (QJsonArray), populating
            results.

     \param key: A QString corresponding to the key used to define the query
     \param value: A QString corresponding to the value used in the query
     \return an integer value denoting the number of search results found
             or -1 in the case of a failed query
    */
    int query(QString value, QString key=QString("title"));

    QJsonObject format(QJsonValue initial);

    QJsonArray format_results(int count);


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
    int request_song(QString download_url, QString target=QString("./"));

};
#endif // SCHANDLER_H
