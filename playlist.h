#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <QObject>
#include <QString>
#include <playlist_item.h>
#include <vector>

class playlist
{

public:
    playlist();
    playlist(QString);
    ~playlist();
    void jumpToSong(QString);
    void jumpToSong(int);
    void addSong(playlist_item);
    void removeSong(playlist_item);
    void moveSong(playlist_item, int);
    QString getActiveSongHash();
    QString getSongHash(int);
    QString getPlaylistTitle();
    QString name();
    std::vector<playlist_item> getSongs();
    playlist_item getSong(int);

public slots:

private:
    //! The playlist title
    /*! The title of the playlist used for displaying purposes. Must be unique. */
    QString title;
    //! The songs for this playlist
    /*! The array of the songs that belong to this playlist in a vector form. */
    std::vector<playlist_item> songs{};
};

#endif // PLAYLIST_H
