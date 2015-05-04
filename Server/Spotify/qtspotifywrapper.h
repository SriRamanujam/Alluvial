#ifndef QtSpotifyWrapper_H
#define QtSpotifyWrapper_H

#include <QThread>
#include <QObject>
#include <libspotify/api.h>
#include <QByteArray>

/*!
 * \brief The QtSpotifyWrapper class  The QtSpotifyWrapper is the class
 * that has the libSpotify implementation.  The class will create it's
 * own thread via QThread which will act as the loop which will process
 * the various libSpotify functions and also keep a reference of the
 * Spotify session.
 * It is important to note that none of this code should only ever be
 * used in the QtSpotifySession class.  This will protect libSpotify
 * from race conditions and unexpected errors related to memory
 * mismanagement, since libSpotify is itself not thread-safe.
 */
class QtSpotifyWrapper: public QThread
{
    Q_OBJECT
public:
    QtSpotifyWrapper();
    ~QtSpotifyWrapper();

    bool FLAG_SPOTIFY_READY;

    bool initSpotify(QString username, QString password);
    void login();
    void logout();
    bool searchSpotify(QString query);
    bool play(bool play, QString linkToSong);
    bool seek(int pos);
    void search_complete_wrapper_cb();
    sp_search *g_search;
    sp_session *g_session;

private:


    int trackPosition;
    int next_timeout;
    QByteArray audioData;
    bool stop;


    void run();


public slots:


signals:
    /*!
     * \brief loggedIn  Signal to emit to the QtSpotifySession class
     * when the user is logged in to Spotify
     */
    void loggedIn(int);

    /*!
     * \brief loggedOut  Signal to emit to the QtSpotifySession class
     * when the user is logged in to Spotify
     */
    void loggedOut();

    /*!
     * \brief searchComplete  Signal to emit to the QtSpotifySession
     * class when the results of a search are completed  and properly
     * formatted as QJsonObjects in a QJsonArray
     */
    void searchComplete(QJsonArray *);

    /*!
     * \brief trackPositionChanged  Signal to emit to the
     * QtSpotifySession class when the playback position on the track
     * has changed
     */
    void playbackEnded();

    /*!
     * \brief playbackEnded  Signal to emit to the QtSpotifySession
     * when playback ends.  This is useful for having the next song
     * load or for knowing that the playback unexpectedly stopped.
     */
    void trackPositionChanged(int);

};

#endif // QtSpotifyWrapper_H
