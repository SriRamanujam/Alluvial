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
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>

/*!
 * \brief The playlist_handler class The main class for media functionality.
 * Controls playback, searches, and navigation.
 *
 *
 * The playlist_handler will be responsible for all media playback and connections made from the c++ to the QML.
 * Contains a vector that will reference all of the made playlists for song playback.
 * Responsible for communication between the Server and the Client and the parsing of JSon.
 * Also handles user interaction to not crash during use.
 */
class playlist_handler:public QObject
{
    Q_OBJECT

public:
    playlist_handler();
    ~playlist_handler();

    void addPlaylist();
    void addPlaylist(playlist);
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

    void setActivePlaylistIndex(int);
    void setActiveSongIndex(int);
    void setShuffle(bool);
    void setRepeat(bool);

    // Taken from the old mediaplayer class
    double getMediaDuration();
    int positionChanged();

public slots:
    void addPlaylist(QVariant);
    void addPlaylist(QString);
    void repeatSwitch();
    void shuffleSwitch();
    void changePlaylist(int);
    void changeTrackListings(int);
    void changeTrackListings(QVariant);
    void changePlaylist(QString);
    void previousSong();
    void nextSong();
    void jumpToSong(QString);
    void jumpToSong(int);
    void metaDataChanged();
    void metaDataChanged(QString, QVariant);
    void dropSong(int);
    void addSong(QVariant, QVariant, QVariant, QVariant, int, QVariant, int);
    void onSearchReceived(QJsonObject);
    void savePlaylist(int);
    void getAllPlaylists(QJsonArray);
    void playSingleSong(QVariant, QVariant, QVariant, QVariant, int, QVariant, int);
    void playAlbum(QVariant);
    void updatePositionWhileRewinding();
    void updatePositionWhileFastForwarding();

    // Taken from the old mediaplayer class
    void play(QByteArray);
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
    //! Displays the tracks of the currently active playlist in the Playlist Panel List View
    void setTrackListings(QVariant);
    //! Displays the playlists in the Playlist Panel Combo Box
    void setPlaylistListings(QVariant);
    //! Change the active song so the Playlist Panel List View highlights the active song
    void changeActiveSong(QVariant);
    //! Display the meta data of the active song for the Item Detail View
    void displayData(QVariant);
    //! Signal the Item Detail View to show the loading.gif
    void loadingSong();
    //! Signal the communications handler to search for the string given
    void requestSong(QString);
    //! Display the search results in the Search Song Results View
    void displaySearchResults(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant);

    //! Display all albums that were in the search results to the Search Album Results View
    void displayAlbumsFromSearchResults(QVariant, QVariant);

    //! Signal the Playlist Panel Combo Box to select the active playlist
    void activePlaylistChanged(QVariant);
    //! Signal the Playback Bar to show the play screen when a song has loaded
    void playSong();
    //! Signal the Playback Bar to show the pause screen when a song is being requested
    void songPaused();

    // Taken from the old mediaplayer class
    //! When the duration of the song has changed, signal the Playback Bar so it displays the length of the song
    void durationChanged(QVariant);
    //! When the playlist_handler is notified of the media player changing song position, signal the new time
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
    //! Keeps track of how long the rewind button is held down for.
    /*! When the rewind button is clicked, this timer starts up.
     * When the button is released, the timer pauses and sets the position of the song back
     * based off of the amount of time the timer has elapsed. */
    QTime *timer;
    //! A QTimer that triggers every tenth of a second to update the position of the playback bar
    /*! This QTimer starts a countdown when the rewind button is pressed. Every time the countdown
    * hits 0, it emits a signal which is used to update the position of the media player. */
    QTimer *rewindTimer;
    //! A QTimer that triggers every tenth of a second to update the position of the playback bar
    /*! This QTimer starts a countdown when the fast forward button is pressed. Every time the countdown
     * hits 0, it emits a signal which is used to update the position of the media player. */
    QTimer *ffTimer;

    // Taken from the old mediaplayer class
    //! The media player object
    /*! Controls all of the playback aspects of the actual media player.
     * Used to be in the mediaplayer class. */
    QMediaPlayer *player;

    //! A vector that holds all albums gotten from the search results
    std::vector<playlist> albums;
};

#endif // PLAYLIST_HANDLER_H
