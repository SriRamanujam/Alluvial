#include <QApplication>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QDebug>
#include <QUrl>
#include <QtQml>
#include <QMediaPlayer>
#include "settings_storage.h"
#include <QtQml>
#include "playlist.h"
#include "playlist_handler.h"
#include "communication/communicationhandler.h"
#include "dataobject.h"
#include <QtCore>
#include <thread>
#include <chrono>
#include <qtlibspotify.h>

#define NUM_OF_THREADS 4

bool FLAG_CLOSING;

struct spotify_credentials{
    int thread_id;
    QString username;
    QString password;
};

void *initSpotifyFromMain(void* sp_arguments){
    QtLibSpotify *spotifyObj = new QtLibSpotify("username", "password");

    while(!FLAG_CLOSING){

    }
    spotifyObj->closing();
}

/*! \mainpage About Alluvial
 *
 * \section intro_sec Introduction
 *
 * Alluvial is a media streaming application that allows users to stream media from their private libraries
 * across the internet to where ever the user may be.
 * It also allows querying of music from Spotify and SoundCloud.
 *
 */

int main(int argc, char *argv[])
{
    FLAG_CLOSING = false;
    pthread_t *spotifyThread;
    int spotifyThread_id;

    qmlRegisterSingletonType(QUrl("qrc:/GlobalVars.qml"), "Alluvial.Globals", 1, 0, "Globals");
    qmlRegisterType<Settings_storing>("AlluvialSettings", 0, 1, "ClientSettings");

	QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    app.setOrganizationName("AlluvialAlphaBuild");
    app.setOrganizationDomain("couponbug.com");
    app.setApplicationName("Alluvial");

    Settings_storing *settings = new Settings_storing();
    QVariant username = settings->value("spotifyUserName");
    QVariant password = settings->value("spotifyPassword");
    struct spotify_credentials spotifyCreds;
    engine.rootContext()->setContextProperty("clientSettings", settings);

    spotifyCreds.thread_id=1;
    spotifyCreds.username = username.toString();
    spotifyCreds.password = password.toString();

    playlist_handler *ph = new playlist_handler();
    CommunicationHandler *comm = new CommunicationHandler("http://10.109.136.88:8900");

    QDir homePath = QDir::currentPath() + "/../Alluvial";
    QFile example(homePath.absolutePath() + "/music/GiDeMo/Little Braver (Album Ver.).mp3");
    example.open(QIODevice::ReadOnly);
    QByteArray dat = example.readAll();
    ph->play(dat);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.rootContext()->setContextProperty("clientSettings", settings);
    QObject *root = engine.rootObjects().first();
    QObject *playButton = root->findChild<QObject*>("playButton");
    QObject *playbackSlider = root->findChild<QObject*>("playbackSlider");
    QObject *volSlider = root->findChild<QObject*>("volumeSlider");

    QObject *rwButton = root->findChild<QObject*>("rewindButton");
    QObject *ffButton = root->findChild<QObject*>("fastForwardButton");

    QObject *leftSkip = root->findChild<QObject*>("leftSkipButton");
    QObject *rightSkip = root->findChild<QObject*>("rightSkipButton");
    QObject *shufButton = root->findChild<QObject*>("shuffleButton");
    QObject *repButton = root->findChild<QObject*>("repeatButton");

    QObject *metadata = root->findChild<QObject*>("songMeta");
    QObject *songSearchResults = root->findChild<QObject*>("songListModel");

    QObject *searchBar = root->findChild<QObject*>("searchBar");
    // Initializing the spotify stuff.  This particular is used for testing and SHOULD be changed later on.
    //spotifyThread_id = pthread_create(spotifyThread, NULL, initSpotifyFromMain, (void*)&spotifyCreds);

    /*
     * IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * ("../Alluvial/main.qml")
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     * */
   //Settings* settings = new Settings(application.data());
    QUrl url("file:///home/jefferson/Code/Alluvial/Alluvial");
    QObject *playlistDropDown = root->findChild<QObject*>("dropdownPlaylistOptions");
    QObject *trackListings = root->findChild<QObject*>("trackListings");

    // Play the next song
    QObject::connect(ph, SIGNAL(requestSong(QString)),
        comm, SLOT(sendMediaRequest(QString)));

    // Play a song clicked from the search results
    QObject::connect(songSearchResults, SIGNAL(requestSong(QVariant, QVariant, QVariant, QVariant, int, QVariant, int)),
        ph, SLOT(playSingleSong(QVariant, QVariant, QVariant, QVariant, int, QVariant, int)));

    // Begin playing the song when it gets it
    QObject::connect(comm, SIGNAL(onMediaReceived(QByteArray)),
        ph, SLOT(play(QByteArray)));

    // When the search results are gotten from the communicator, display them
    QObject::connect(comm, SIGNAL(onSearchReceived(QJsonObject)),
        ph, SLOT(onSearchReceived(QJsonObject)));

    // Display the search results
    QObject::connect(ph, SIGNAL(displaySearchResults(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)),
        songSearchResults, SLOT(displaySearchResults(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)));

    // Display the active songs meta data in Item Detail View
    QObject::connect(ph, SIGNAL(displayData(QVariant)),
        metadata, SLOT(infoChanged(QVariant)));

    // Display a small loading screen when songs are changed
    QObject::connect(ph, SIGNAL(loadingSong()),
        metadata, SLOT(loadingSong()));

    // Pause or play the song
    QObject::connect(playButton, SIGNAL(playClicked()),
        ph, SLOT(playOrPause()));

    // When the active song is changed by clicking it in playlist panel, play the new song
    QObject::connect(trackListings, SIGNAL(activeSongChanged(int)),
        ph, SLOT(jumpToSong(int)));

    // Adjust the volume according to the position of the volume slider
    QObject::connect(volSlider, SIGNAL(changeVol(int)),
        ph, SLOT(setVolume(int)));
    volSlider->setProperty("value", 50);

    // Rewind when the rewind button is clicked
    QObject::connect(rwButton, SIGNAL(startRewind()),
        ph, SLOT(startRewind()));

    // Fast forward when the fast forward button is clicked
    QObject::connect(ffButton, SIGNAL(startFastForward()),
        ph, SLOT(startFastForward()));

    // When the rewind button is released, resume the song
    QObject::connect(rwButton, SIGNAL(endRewind()),
        ph, SLOT(resetPlaybackRate()));

    // When the fast forward button is released, resume the song
    QObject::connect(ffButton, SIGNAL(endFastForward()),
        ph, SLOT(resetPlaybackRate()));

    // Skip to position based off of the slider
    QObject::connect(playbackSlider, SIGNAL(playbackPosChanged(int)),
        ph, SLOT(skipTo(int)));

    // When the song ends, work on loading the next song
    QObject::connect(playbackSlider, SIGNAL(songFinished()),
        ph, SLOT(nextSong()));

    // Progress the music bar based off of the song position
    QObject::connect(ph, SIGNAL(positionChanged(QVariant)),
        playbackSlider, SLOT(positionChanged(QVariant)));

    // When the songs' data loads and the duration changes, fix the labels displayed
    QObject::connect(ph, SIGNAL(durationChanged(QVariant)),
        playbackSlider, SLOT(durationChanged(QVariant)));

    // Toggle the shuffle button when clicked
    QObject::connect(shufButton, SIGNAL(clicked()),
        ph, SLOT(shuffleSwitch()));

    // Toggle the repeat button when clicked
    QObject::connect(repButton, SIGNAL(clicked()),
        ph, SLOT(repeatSwitch()));

    // Jump to the previous song when clicked
    QObject::connect(leftSkip, SIGNAL(clicked()),
        ph, SLOT(previousSong()));

    // Skip to the next song
    QObject::connect(rightSkip, SIGNAL(clicked()),
        ph, SLOT(nextSong()));

    // Change what songs are active when the active playlist is changed
    QObject::connect(playlistDropDown, SIGNAL(activePlaylistChanged(int)),
        ph, SLOT(changeTrackListings(int)));

    // When the active playlist is changed by the playlist handler, display the active playlist
    QObject::connect(ph, SIGNAL(activePlaylistChanged(QVariant)),
        playlistDropDown, SLOT(playlistChanged(QVariant)));

    // Display all songs when the active playlist songs' are changed
    QObject::connect(ph, SIGNAL(setTrackListings(QVariant)),
        trackListings, SLOT ( changeListings(QVariant)));

    // Display all the playlists in the drop down menu when a playlist is added or dropped
    QObject::connect(ph, SIGNAL(setPlaylistListings(QVariant)),
        playlistDropDown, SLOT ( changePlaylistListings(QVariant)));

    // Change which song is highlighted when the active song changes
    QObject::connect(ph, SIGNAL(changeActiveSong(QVariant)),
        trackListings, SLOT (changeActiveSong(QVariant)));

    // Delete a song from the active playlist when it is closed
    QObject::connect(trackListings, SIGNAL(deleteSongFromPlaylist(int)),
        ph, SLOT (dropSong(int)));

    // When a song is added to the playlist from the search screen, add it to the active playlist in the handler
    QObject::connect(songSearchResults, SIGNAL(addSongToPlaylist(QVariant, QVariant, QVariant, QVariant, int, QVariant, int)),
        ph, SLOT (addSong(QVariant, QVariant, QVariant, QVariant, int, QVariant, int)));

    // Submit a query to the communicator when a search is submitted
    QObject::connect(searchBar, SIGNAL(searchQuery(QVariant)),
        comm, SLOT(sendSearchRequest(QVariant)));

    playlist_item *newSong0 = new playlist_item("#0", "song 0", 5);
    playlist_item *newSong1 = new playlist_item("#1", "song 1", 5);
    playlist_item *newSong2 = new playlist_item("#2", "song 2", 5);
    playlist_item *newSong3 = new playlist_item("#3", "song 3", 5);
    playlist_item *newSong4 = new playlist_item("#4", "song 4", 5);
    playlist_item *newSong5 = new playlist_item("#5", "song 5", 5);
    playlist_item *newSong6 = new playlist_item("#6", "song 6", 5);
    playlist_item *newSong7 = new playlist_item("#7", "song 7", 5);
    playlist_item *newSong8 = new playlist_item("#8", "song 8", 5);
    playlist_item *newSong9 = new playlist_item("#9", "song 9", 5);

    ph->addPlaylist("Playlist 1");
    ph->addSong(0, *newSong0);
    ph->addSong(0, *newSong1);
    ph->addSong(0, *newSong2);
    ph->addPlaylist("Playlist 2");
    ph->addSong(1, *newSong3);
    ph->addSong(1, *newSong4);
    ph->addPlaylist("Playlist 3");
    ph->addSong(2, *newSong5);
    ph->addSong(2, *newSong6);
    ph->addSong(2, *newSong7);
    ph->addSong(2, *newSong8);

	QStringList playlists;
    for ( int index = 0; index < ph->getPlaylists().size(); index++ )
    {
        playlists.append(ph->getPlaylist(index).getPlaylistTitle());
    }

    ph->changeTrackListings(0);

    engine.rootContext()->setContextProperty("playlistModel", QVariant::fromValue(playlists));

	int appInt = app.exec();

    return appInt;
}
