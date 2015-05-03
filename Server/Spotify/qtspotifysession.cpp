#include "qtspotifysession.h"
#include "../Settings/settings_storage.h"

QtSpotifySession::QtSpotifySession()
{
    wrapper = new QtSpotifyWrapper();

    connect(wrapper, SIGNAL(loggedIn(int)),
            this, SIGNAL(loggedIn(int)));
    connect(wrapper, SIGNAL(loggedOut()),
            this, SIGNAL(loggedOut()));
    connect(wrapper, SIGNAL(searchComplete(QJsonArray *)),
            this, SIGNAL(searchResultReady(QJsonArray *)));
    //connect(wrapper, SIGNAL(logMessage(QString)),
            //this, SIGNAL(logMessage(QString)));
    connect(wrapper, SIGNAL(trackPositionChanged(int)),
            this, SIGNAL(trackPositionChanged(int)));
    connect(wrapper, SIGNAL(playbackEnded()),
            this, SIGNAL(playbackEnded()));
    //connect(wrapper, SIGNAL(coverReady(QImage *)),
            //this, SIGNAL(coverReady(QImage *)));
    //connect(wrapper, SIGNAL(connectionError(int)),
            //this, SIGNAL(connectionError(int)));
}

QtSpotifySession::~QtSpotifySession()
{

}

bool QtSpotifySession::initSpotify()
{
    Settings_storing *settings = new Settings_storing;
    QString username = settings->value("spotifyUserName").toString();
    QString password = settings->value("spotifyPassword").toString();

    return wrapper->initSpotify(username, password);
}
void QtSpotifySession::login()
{
    wrapper->login();
}

void QtSpotifySession::logout()
{
    wrapper->logout();
}

bool QtSpotifySession::startSearch(QString query)
{
    return wrapper->searchSpotify(query);

}

bool QtSpotifySession::play(bool play)
{
    return wrapper->play(play);
}

bool QtSpotifySession::seek(int pos)
{
    return wrapper->seek(pos);
}
//

void QtSpotifySession::playMusic(){
    //unsure what to put here right now
}
