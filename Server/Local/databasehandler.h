#ifndef DatabaseHandler_H
#define DatabaseHandler_H

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/tstring.h>
#include <QDebug>
#include <QDir>
#include <QMimeDatabase>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QDirIterator>
#include <stdlib.h>
#include <QStringList>
using std::string;

//struct that contains needed metadata in QString and uint
/*!
 * \brief The MD struct holds all the metadata associated with a particular song.
 *
 * This struct is partially populated by taglib during metadata querying operations,
 * with the exception of the PKID field, which is populated by the DatabaseHandler
 * with the primary database key of the file.
 */
struct MD{

    /*!
     * \brief Path to the song file on disk.
     */
    QString filepath;
    /*!
     * \brief Artist name. Taken from "artist" tag on file.
     */
    QString artist;
    /*!
     * \brief Album name. Taken from "album" tag on  file.
     */
    QString album;
    /*!
     * \brief Song title. Taken from "title" tag on file.
     */
    QString title;
    /*!
     * \brief Genre. Taken from "genre" tag on file.
     */
    QString genre;
    /*!
     * \brief Primary key of this song in the database.
     */
    signed int PKID;
    /*!
     * \brief Track number. Taken from "track_number" tag on file.
     */
    signed int track_num;
    /*!
     * \brief This and length_sec comprise the running time of the song.
     */
    signed int length_min;
    /*!
     * \brief This and length_min comprise the running time of the song.
     */
    signed int length_sec;
};

/*!
 * \brief This DatabaseHandler class contains all database functions from creating them, to their
 * tables, filing them with data to retrieving their contents.
 */
class DatabaseHandler : public QObject
{
public:
    DatabaseHandler();
    ~DatabaseHandler();
    MD ripMetaData(QString file);
    bool isMusic (QString const file);
    bool DBInit();
    bool openDB();
    bool createTable();
    void DBpopulate(QDir dir);
    std::vector <MD> queryDB(QString query);
    QString getSongFP(int PKID);
    bool insertPlaylist(QString name, QString filepath);
    QString retrievePlaylist(QString name);
    QStringList retrieveAllPlaylists();

    /*!
     * \brief this global variable is needed so multiple functions
     * \       can execute commands on the same sqlquery (i.e. open db, create tables, populate db)
     */
    QSqlQuery query;
    /*!
     * \brief global db variable that all db code acts on.
     */
    QSqlDatabase db;
private:

};


#endif // DatabaseHandler_Her
