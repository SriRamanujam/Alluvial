#ifndef PLAYLISTHANDLER_H
#define PLAYLISTHANDLER_H
#include "JSONhandler.h"
#include <QJsonDocument>

/*!
 * \brief The playListHandler class handles the saving and
 * retrieving of playlists.
 */
class playListHandler
{
public:
    playListHandler();
    ~playListHandler();
    bool setPlaylist(QJsonObject playlist);
    QJsonObject getPlaylist(QString playListName);
    QJsonArray getAllPlaylist();

};

#endif // PLAYLISTHANDLER_H
