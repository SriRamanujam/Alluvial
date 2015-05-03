#include "playlisthandler.h"

playListHandler::playListHandler()
{

}

playListHandler::~playListHandler()
{

}
/*!
 * \brief Function is passed a QJsonObject representing a playList
 * it turns it into a QJsonDocument. A json file is created
 * in the filesystem and saved. A database entry representing
 * this file is made.
 *
 * \param playlist QJsonObject representing a playList
 * \return true if playlist set was successful
 */
bool playListHandler::setPlaylist(QJsonObject playlist)
{

    QString json = ".json";
    QString name = QString(playlist["name"].toString());
    QString fp = "/home/moe/Desktop/playlist/";
    fp.append(name);
    fp.append(json);
    QJsonDocument *insert = new QJsonDocument(playlist);
    QFile jsonFile(fp);
    jsonFile.open(QFile::WriteOnly);
    bool writeSuccess = jsonFile.write(insert->toJson());
    DatabaseHandler *db = new DatabaseHandler();
    bool opendb = db->openDB();
    bool dbRet = db->insertPlaylist(name, fp);

    return writeSuccess;
}
/*!
 * \brief Given the name, gets the json file representing the playList
 * goes into the db to find the filepath for the playlist json file
 * turns that nto a QJsonDocument and then into a QJsonObject which it returns.
 * \param  QString representing the queryed playlist
 * \return QJsonObject representing the playlist query
 */
QJsonObject playListHandler::getPlaylist(QString playListName)
{
    QJsonObject test;
    DatabaseHandler *db = new DatabaseHandler();
    bool opendb = db->openDB();
    QString result = db->retrievePlaylist(playListName);
    QString val;
    QFile JSONfile;
    JSONfile.setFileName(result);
    JSONfile.open(QIODevice::ReadOnly | QIODevice::Text);
    val = JSONfile.readAll();
    JSONfile.close();
    QJsonDocument JSONDoc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject JSONObj = JSONDoc.object();
    return JSONObj;
}

QJsonArray playListHandler::getAllPlaylist()
{
    QJsonArray resultArray;
    DatabaseHandler *db = new DatabaseHandler();
    bool opendb = db->openDB();
    QStringList result = db->retrieveAllPlaylists();
    foreach (QString path, result) {
        QString val;
        QFile JSONfile;
        JSONfile.setFileName(path);
        JSONfile.open(QIODevice::ReadOnly | QIODevice::Text);
        val = JSONfile.readAll();
        JSONfile.close();
        QJsonDocument JSONDoc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject JSONObj = JSONDoc.object();
        resultArray.append(JSONObj);
    }

    return resultArray;
}





