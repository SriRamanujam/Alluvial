#ifndef QTLIBSPOTIFY_H
#define QTLIBSPOTIFY_H

#include <libspotify/api.h>
#include <QObject>



class QtLibSpotify:public QObject
{
public:
    bool FLAG_SPOTIFY_READY;

    QtLibSpotify(QString username, QString password);
    ~QtLibSpotify();
    int initSpotify(QString username, QString password);
    sp_search *searchSpotify(sp_session* session, QString searchQuery);
    void playSongSpotify(sp_session* session, sp_track* track);
    sp_connectionstate GetConnectionState();

    bool isLoggedIn();
public slots:
    sp_error spotifyLogin(QString username, QString password);
    sp_error spotifyLogout(sp_session *user_session);
    sp_error releaseSpotifySession(sp_session *user_session);
    sp_error search(QString searchString);
    sp_error closing();

};

#endif // QTLIBSPOTIFY_H
