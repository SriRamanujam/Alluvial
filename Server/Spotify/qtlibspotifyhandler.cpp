#include "qtlibspotifyhandler.h"
#include "qtlibspotify.h"
#include "../Settings/settings_storage.h"
#include <QThread>
#include <QDebug>

//QtLibSpotify *objlibSpotify;
QThread *spotifyThread;


/*!
 * \brief QtLibSpotifyHandler::QtLibSpotifyHandler  This is the default constructor for the
 * This class allows access to the QtLibSpotify class.  This is an in-between which allows
 * enough abstraction to simpify the access of libSpotify and allows the QtLibSpotify class
 * to run on its own thread.
 * \param parent  This This will be generated automatically. Just don't mess with it.
 */
QtLibSpotifyHandler::QtLibSpotifyHandler(QObject *parent) : QObject(parent)
{
    //Settings_storing *settings = new Settings_storing;
    //uname = (settings->value("spotifyUserName")).toString();
    //pword = (settings->value("spotifyPassword")).toString();
    QtLibSpotifyWorker *spotifyWorker = new QtLibSpotifyWorker();
    spotifyWorker->moveToThread(&spotifyThread);
    connect(&spotifyThread, &QThread::started, spotifyWorker, &QtLibSpotifyWorker::doWork);
    connect(&spotifyThread, &QThread::finished, spotifyWorker, &QObject::deleteLater);
    connect(this, &QtLibSpotifyHandler::startWorking, spotifyWorker, &QtLibSpotifyWorker::doWork);
    connect(spotifyWorker, &QtLibSpotifyWorker::returnDataStream, this, &QtLibSpotifyHandler::dataStreamCB);
    connect(this, &QtLibSpotifyHandler::searchForSpotify, spotifyWorker, &QtLibSpotifyWorker::incomingSearch);
    //connect(spotifyWorker, &QtLibSpotifyWorker::searchSpotify)
    spotifyThread.start();
}

/*!
 * \brief QtLibSpotifyHandler::~QtLibSpotifyHandler  The deconstructor for the QtLibSpotifyHandler class
 *
 * This will signal to QtLibSpotify to release any spotify structs which have not been released already.
 * Typically this will be called upon the program closing.
 */
QtLibSpotifyHandler::~QtLibSpotifyHandler()
{

}
/*!
 * \brief QtLibSpotifyHandler::dataStreamCB  The callback for the music stream that spotify will give out.
 * This is a slot that will get the stream of music as a QByteArray and will pass it out of the QtLibSpotifyHandler.
 * \param datastream The QByteArray that is the music stream
 */
void QtLibSpotifyHandler::dataStreamCB(QByteArray datastream)
{

}
/*!
 * \brief QtLibSpotifyHandler::searchResultsCB The callback for the search results that Spotify will give out.
 *
 * \param searchResults  The QJsonArray of search results that Spotify gave back.  These correspond with the
 * input received from the SearchBar
 */
void QtLibSpotifyHandler::searchResultsCB(QJsonArray searchResults)
{

}
/*!
 * \brief QtLibSpotifyHandler::searchSlot  The slot which gets the input from the SearchBar in the Main UI.
 * \param query The QString input from the user via the SearchBar
 */
void QtLibSpotifyHandler::searchSlot(QString query){
    qDebug() << "QtLibSpotifyhandlerSearch: searchSlot has received a query!";
    //searchQtlibSpotify(query);
    emit searchForSpotify(query);
}
