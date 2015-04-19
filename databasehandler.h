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
using std::string;

//struct that contains needed metadata in QString and uint
struct MD{

    QString filepath, artist, album, title, genre;
    signed int PKID, track_num, length_min, length_sec;
};


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
    QSqlQuery query;
    QSqlDatabase db;
private:

};


#endif // DatabaseHandler_H
