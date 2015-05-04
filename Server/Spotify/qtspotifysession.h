#ifndef QTSPOTIFYSESSION_H
#define QTSPOTIFYSESSION_H

#include "qtspotifywrapper.h"
#include <QObject>

/*!
 * \brief The QtSpotifySession class  This class is used to interact with libSpotify
 *  and the functionality it provides.  The point of this class is to abstract away
 * the Spotify features to simplify the calls that come from the mediaHandler class.
 *
 * You can imagine this class as an outer layer of the libSpotify implementation.
 */
class QtSpotifySession: public QObject
{
    Q_OBJECT
public:
    QtSpotifySession();
    ~QtSpotifySession();

    bool initSpotify();
    void login();
    void logout();
    bool startSearch(QString query);
    bool getMedia(bool play, QString linkToSpotify);
    bool seek(int pos);

private:
    QtSpotifyWrapper *wrapper;

signals:

    /*!
     * \brief loggedIn  Signal to emit when the user is logged in to Spotify
     */
    void loggedIn(int);

    /*!
     * \brief loggedOut  Signal to emit when the user is logged in to Spotify
     */
    void loggedOut();

    /*!
     * \brief searchResultReady  Signal to emit when the results of a search are completed
     * and properly formatted as QJsonObjects in a QJsonArray
     * \param obj  The QJsonArray that contains the tracks that the Spotify returned
     */
    void searchResultReady(QJsonArray *obj);

    /*!
     * \brief trackPositionChanged  Signal to emit when the playback position on the track
     * has changed
     */
    void trackPositionChanged(int);
    /*!
     * \brief playbackEnded  Signal to emit when playback ends.  This is useful for having
     * the next song load or for knowing that the playback unexpectedly stopped.
     */
    void playbackEnded();
};

#endif // QTSPOTIFYSESSION_H
