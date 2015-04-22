#ifndef QTLIBSPOTIFYHANDLER_H
#define QTLIBSPOTIFYHANDLER_H

#include <QString>
#include <QObject>
#include <QJsonArray>
#include <QThread>
#include <QDebug>
#include "settings_storage.h"
#include <libspotify/api.h>
#include "qtlibspotify.h"


class QtLibSpotifyWorker : public QObject
{
    Q_OBJECT

public:
    QtLibSpotifyWorker(){};
    ~QtLibSpotifyWorker(){};
public slots:
    void doWork(){
        Settings_storing *settings = new Settings_storing();
        //QString spotifyusername = settings->value("spotifyUserName").toString();
        //QString spotifypassword = settings->value("spotifyPassword").toString();
        QString result;
        QtLibSpotify *spotifyObj = new QtLibSpotify( "username", "password");
        qDebug() << "Made it inside the doWork function";
    }

signals:
    void finished();

    void returnDataStream(QByteArray &musicStream);
    void returnSearchResults(QJsonArray &searchResults);
};

class QtLibSpotifyHandler : public QObject
{
    Q_OBJECT
    QThread spotifyThread;

public:
    QString uname;
    QString pword;
    QtLibSpotifyHandler(QObject *parent = 0);
    QtLibSpotifyHandler(QString username, QString password);
    ~QtLibSpotifyHandler();

    void searchQtlibSpotify(QString searchString);

signals:
    void playbackSignal(QByteArray musicalNotesThatPlayInASequence);
    void onSearchComplete(QJsonArray searchResults);
    void startWorking(QString username, QString password);

public slots:
    void searchResultsCB(QJsonArray jsonSearchResults);
    void dataStreamCB(QByteArray byteArray);
};

#endif // QTLIBSPOTIFYHANDLER_H
