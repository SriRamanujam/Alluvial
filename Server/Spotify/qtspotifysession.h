#ifndef QTSPOTIFYSESSION_H
#define QTSPOTIFYSESSION_H

#include "qtspotifywrapper.h"
#include <QObject>

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
    bool play(bool play);
    bool seek(int pos);

public slots:
    //void search(QString searchQuery);
    //not sure what to pass to spotify for song ref
    //just yet...
    void playMusic();


private:
    QtSpotifyWrapper *wrapper;

signals:
    void loggedIn(int);
    void loggedOut();
    //QUESTION:  should I be passing something back with this?
    void searchResultReady(QJsonArray *obj);
    void trackPositionChanged(int);
    void playbackEnded();


};

#endif // QTSPOTIFYSESSION_H
