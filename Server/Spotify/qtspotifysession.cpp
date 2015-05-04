#include "qtspotifysession.h"
#include "../Settings/settings_storage.h"

/*!
 * \brief QtSpotifySession::QtSpotifySession The constructor for QtSpotifySession.
 * It will create a reference to the wrapper around libSpotify.  Then it creates
 * connections between the wrapper and this class.
 */
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
/*!
 * \brief QtSpotifySession::~QtSpotifySession  The deconstructor for QtSpotifySession.
 * It doesn't do anything.
 */
QtSpotifySession::~QtSpotifySession()
{

}

/*!
 * \brief QtSpotifySession::initSpotify  calls the function in the wrapper class that will
 * begin the series of steps necessary to start an instance of a libSpotify and connect to
 * Spotify's servers.
 * \return A boolean value representing the success or failure of the functions execution.
 */
bool QtSpotifySession::initSpotify()
{
    Settings_storing *settings = new Settings_storing;
    QString username = settings->value("spotifyUserName").toString();
    QString password = settings->value("spotifyPassword").toString();

    return wrapper->initSpotify(username, password);
}
/*!
 * \brief QtSpotifySession::startSearch  Tells the wrapper class to search, using libSpotify,
 * for a specific QString value.
 * \param query  The value that will be used for the libSpotify search
 * \return A boolean representing the success or failure of the search executed by the
 * wrapper class to libSpotify
 */
bool QtSpotifySession::startSearch(QString query)
{
    return wrapper->searchSpotify(query);

}
/*!
 * \brief QtSpotifySession::getMedia  The function that tells the wrapper class to play
 * the track that was selected by the user in the client.
 * \param play  A boolean which will tell libSpotify to either play (true, 1) or
 * pause (false, 0) the track specified by linkToSpotify.
 * \param linkToSpotify  The QString representation of a spotify URI.
 * An example - spotify:track:xxxxxxxxxxxxxxxxxxxxxx
 * Note: the number of X's above is not the correct amount of characters that will
 * appear in a real URI from spotify.
 * \return A boolean representing the success or failure of the function
 */
bool QtSpotifySession::getMedia(bool play, QString linkToSpotify)
{
    return wrapper->play(play, linkToSpotify);
}
/*!
 * \brief QtSpotifySession::seek  This function tells the libSpotify wrapper class that
 * playback must be modified to reflect a change in the position of the pointer to the
 * current playback location on the track.
 * \param pos  The new position which the playback should now start playing from.
 * \return A boolean representing the success or failure of the function.
 */
bool QtSpotifySession::seek(int pos)
{
    return wrapper->seek(pos);
}
