#ifndef PLAYLIST_HANDLER_H
#define PLAYLIST_HANDLER_H
#include <vector>
#include <playlist.h>
#include <QObject>
#include <QStringList>
#include <QQmlApplicationEngine>
#include <QByteArray>
#include <QMediaPlayer>
#include <QObject>
#include <QTime>

class playlist_handler:public QObject
{
    Q_OBJECT

public:
    playlist_handler();
    ~playlist_handler();

    void addPlaylist();
    void addPlaylist(playlist);
    void addPlaylist(QString);
    void addSong(int, playlist_item);
    void dropPlaylists();
    void dropPlaylist(playlist);
    void dropPlaylist(QString);
    void dropSong(int, playlist_item);

    std::vector<playlist> getPlaylists();
    playlist getPlaylist(int);
    std::vector<QString> getPlaylistNames();
    std::vector<QString> getPlaylistSongNames(int);
    playlist getActivePlaylist();
    int getActivePlaylistIndex();
    int getActiveSongIndex();
    QString getActiveSongHash();
    bool getShuffle();
    bool getRepeat();

    void jumpToSong(QString);
    void jumpToSong(int);
    void setActivePlaylistIndex(int);
    void setActiveSongIndex(int);
    void setShuffle(bool);
    void setRepeat(bool);

    // Taken from the old mediaplayer class
    void play(QByteArray);
    double getMediaDuration();
    int positionChanged();

public slots:
    void repeatSwitch();
    void shuffleSwitch();
    void changePlaylist(int);
    void changeTrackListings(int);
    void changePlaylist(QString);
    void previousSong();
    void nextSong();

    // Taken from the old mediaplayer class
    void play();
    void pause();
    void skipTo(int);
    void playOrPause();
    void setVolume(int);
    void startRewind();
    void startFastForward();
    void resetPlaybackRate();
    void childPositionChanged(qint64);
    void childDurationChanged();

signals:
    void setTrackListings(QVariant);
    void setPlaylistListings(QVariant);
    void changeActiveSong(QVariant);

    // Taken from the old mediaplayer class
    void durationChanged(QVariant);
    void positionChanged(QVariant);

private:
    //! All of the playlists
    /*! A vector containing all of the playlists that are saved on the server, but displayed on the client.
     * The saved info on the client is barebones for displaying purposes. Info is retrieved when a playlist
     * is selected. */
    std::vector<playlist> playlists;
    //! The active playlist index
    /*! The index of the active playlist in the vector of all saved playlists. */
    int activePlaylist;
    //! The active song index
    /*! The index of the active song in the vector of a playlists' songs */
    int activeSong;
    //! A boolean value for the shuffle setting
    /*! Indicates whether to shuffle through songs when next song is clicked, or to iterate through */
    bool shuffle;
    //! A boolean value for the repeat setting
    /*! Indicates whether to repeat the same song when next song is clicked, or to iterate through */
    bool repeat;

    // Taken from the old mediaplayer class
    //! The media player object
    /*! Controls all of the playback aspects of the actual media player.
     * Used to be in the mediaplayer class. */
    QMediaPlayer *player;
};

#endif // PLAYLIST_HANDLER_H
