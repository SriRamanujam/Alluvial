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

    QDir homePath = QDir::currentPath() + "/../Alluvial";
    QFile example(homePath.absolutePath() + "/music/GiDeMo/Rain Song.mp3");
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
    QObject *trackModel = root->findChild<QObject*>("trackModel");

    // Pause or play the song
    QObject::connect(playButton, SIGNAL(playClicked()),
        ph, SLOT(playOrPause()));

    // Adjust the volume according to the position of the volume slider
    QObject::connect(volSlider, SIGNAL(changeVol(int)),
        ph, SLOT(setVolume(int)));
    volSlider->setProperty("value", 50);

    QObject::connect(rwButton, SIGNAL(startRewind()),
        ph, SLOT(startRewind()));

    QObject::connect(ffButton, SIGNAL(startFastForward()),
        ph, SLOT(startFastForward()));

    QObject::connect(rwButton, SIGNAL(endRewind()),
        ph, SLOT(resetPlaybackRate()));

    QObject::connect(ffButton, SIGNAL(endFastForward()),
        ph, SLOT(resetPlaybackRate()));

    // Skip to position based off of the slider
    QObject::connect(playbackSlider, SIGNAL(playbackPosChanged(int)),
        ph, SLOT(skipTo(int)));

    // Progress the music bar based off of the song position
    QObject::connect(ph, SIGNAL(positionChanged(QVariant)),
        playbackSlider, SLOT(positionChanged(QVariant)));

    QObject::connect(ph, SIGNAL(durationChanged(QVariant)),
        playbackSlider, SLOT(durationChanged(QVariant)));

    QObject::connect(shufButton, SIGNAL(clicked()),
        ph, SLOT(shuffleSwitch()));

    QObject::connect(repButton, SIGNAL(clicked()),
        ph, SLOT(repeatSwitch()));

    QObject::connect(leftSkip, SIGNAL(clicked()),
        ph, SLOT(previousSong()));

    QObject::connect(rightSkip, SIGNAL(clicked()),
        ph, SLOT(nextSong()));

    QObject::connect(playlistDropDown, SIGNAL(activePlaylistChanged(int)),
        ph, SLOT(changeTrackListings(int)));

    QObject::connect(ph, SIGNAL(setTrackListings(QVariant)),
        trackListings, SLOT ( changeListings(QVariant)));

    QObject::connect(ph, SIGNAL(setPlaylistListings(QVariant)),
        playlistDropDown, SLOT ( changePlaylistListings(QVariant)));

    QObject::connect(ph, SIGNAL(changeActiveSong(QVariant)),
        trackListings, SLOT (changeActiveSong(QVariant)));

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
