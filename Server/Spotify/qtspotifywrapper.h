#ifndef QtSpotifyWrapper_H
#define QtSpotifyWrapper_H

#include <QThread>
#include <QObject>
#include <libspotify/api.h>
#include <QByteArray>


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
    bool play(bool play);
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
    void loggedIn(int);
    void loggedOut();
    //QUESTION: again should I be passing data back  with this signal????
    void searchComplete(QJsonArray *);
    void playbackEnded();
    void trackPositionChanged(int);

};

#endif // QtSpotifyWrapper_H
