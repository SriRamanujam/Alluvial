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
    void setTrackListings(QStringList list);

    // Taken from the old mediaplayer class
    void durationChanged(qint64);
    void positionChanged(QVariant);

private:
    std::vector<playlist> playlists; /**< Vector of all playlist structures */
    int activePlaylist; /**< The index of the current active playlist */
    int activeSong; /**< The index of the current song that is playing */
    bool shuffle; /**< Boolean indicator - shuffle activated */
    bool repeat; /**< Boolean indicator - repeat activated */
    QTime timer;

    // Taken from the old mediaplayer class
    QMediaPlayer *player;
};

#endif // PLAYLIST_HANDLER_H
