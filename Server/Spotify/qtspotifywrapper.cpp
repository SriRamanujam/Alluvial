#define USER_AGENT "AlluvialMusicPlayer1"

#include "qtspotifywrapper.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <pthread.h>
#include "../../appkey.h"


int timeout;
static sp_session *session;

//Synchronization variable to tell main thread ot process spotify events
static int notify_events;
static pthread_mutex_t notify_mutex;
static pthread_cond_t notify_cond;

/*!
 * \brief QtSpotifyWrapper::QtSpotifyWrapper  The constructor for the QtSpotifyWrapper
 * class.  It will initialize a few key variables that will be used in the rest of the
 * class
 */
QtSpotifyWrapper::QtSpotifyWrapper()
{
    FLAG_SPOTIFY_READY = false;
    timeout = -1;
}
/*!
 * \brief QtSpotifyWrapper::~QtSpotifyWrapper  The deconstructor of the QtSpotifyWrapper
 * class.  It will release the any sessions that were created by the class during execution.
 */
QtSpotifyWrapper::~QtSpotifyWrapper()
{
    sp_session_release(g_session);
    sp_session_release(session);

}

/*!
 * \brief spotifyTrackParser  This is helper function that will convert a sp_track *
 * object into a QJsonValue object.  This is done so that the metadata of the track
 * may be passed back to the client and a QJsonvalue is used as a standardized format
 * for all metadata.
 * \param track  The track whose metadata will be formatted into a QJsonValue
 * \param index
 * \return
 */
static QJsonValue spotifyTrackParser(sp_track * track, int index)
{
    QString artistString;
    QString duration;
    QObject spotifyJsonObj;
    QString linkStr;

    sp_album * spotifyAlbum = sp_track_album(track);
    sp_link *trackLink = sp_link_create_from_track(track, 0);



    for(int j = 0; j < sp_track_num_artists(track); j++){
        sp_artist *tmpArtist = sp_track_artist(track,j);
        if(sp_track_num_artists(track) > 1){
            artistString.append(sp_artist_name(tmpArtist));
            artistString.append(", ");
        }
    }

    int durationInSecs = (sp_track_duration(track))/1000;
    QString trackSeconds = QString::number(durationInSecs%60);
    QString trackMinutes = QString::number(durationInSecs/60);
    //QString::number((int)(durationInSecs/60));
    if(durationInSecs%60 < 10){
        trackSeconds = "0" + trackSeconds;
    }
    duration = trackMinutes + ":" + trackSeconds;

    sp_link * link = sp_link_create_from_track(track,0);

     char spotifyTmpURI[256];

     if(0 > sp_link_as_string(trackLink, spotifyTmpURI, sizeof(spotifyTmpURI))){
         linkStr = "error";
     } else {
         linkStr = QString::fromUtf8(spotifyTmpURI, sizeof(spotifyTmpURI));
     }


    QJsonObject media{
        {"hash", linkStr},
        {"order", ""},
    };

    QJsonObject spotifyMeta{
        {"title", sp_track_name(track)},
        {"album", sp_album_name(spotifyAlbum)},
        {"artist", artistString},
        {"track_number", sp_track_index(track)},
        {"length", duration}, //get from fucking duration
        {"genre", ""}
    };

    media["metadata"] = spotifyMeta;

    qDebug() << "Spotify Output:" << spotifyMeta;

    return QJsonValue(media);
}
/*!
 * \brief connection_error  This callback will display a debug comment about
 * any problems with the connection with Spotify
 * \param session  The session that was created when the server is initialized
 * \param error  The error code about the connection problem
 */
static void  connection_error(sp_session *session, sp_error error){
    qDebug() << "Spotify:  Connection Error - " << sp_error_message(error) << " - for user - " << sp_session_user(session);
}
/*!
 * \brief logged_in  The callback that will create the playlist container
 * for the session
 * \param session The session that was created when the server is initialized
 * \param error  The result of trying to log into libSpotify.
 */
static void logged_in(sp_session *session, sp_error error){
    qDebug() << "Spotify: Logged_in Reached";
    if(SP_ERROR_OK != error){
        //FLAG_SPOTIFY_STATUS=false;
        qDebug() << "Spotify:  Log in failed: " << sp_error_message(error);
    }
    else{
        qDebug() << "Spotify: Log in successful for user - " << sp_session_user(session);

        sp_session_playlistcontainer(session);
    }

}
/*!
 * \brief logged_out  The callback that is called when the user is logged out of
 * Spotify.  It is used primarily for debugging any issues with libSpotify
 * connections
 * \param session  The session that was created when the server is initialized
 */
static void logged_out(sp_session *session){
    qDebug() << "Spotify: Logged Out for user - " << sp_session_user(session);
}
/*!
 * \brief log_message This is a callback that will output a message that Spotify
 * has internally created and wants to output.
 * \param session  The session that was created when the server is initialized
 * \param data  The string that libSpotify wants to output to the user
 */
static void log_message(sp_session *session, const char *data){
    qDebug() << "Spotify: log_message for user " << sp_session_user(session) << " - "  << data;
}

/*!
 * \brief notify_main_thread  This will signal to the loop in the "run()" function
 * that it should start executing again.
 * Pretty sure this is actually pointless
 * More research needed.
 * \param session The session that was created when the server is initialized
 */
static void notify_main_thread(sp_session *session){
    qDebug() << "notify_main_thread callback received";
    pthread_mutex_lock(&notify_mutex);
    notify_events = 1;
    pthread_cond_signal(&notify_cond);
    pthread_mutex_unlock(&notify_mutex);
}

/*! \brief SP_CALLCONV search_complete The callback for when Spotify finsihes a
 * search.  This is called automatically by libspotify once the specified search is
 * completed.
 * \param search  The search object created with sp_search_create.  It is a
 * struct, created by spotify, that holds all results from the aforementioned search
 * \param userdata  A reference to the obj that search function was called from
 */
static void SP_CALLCONV search_complete(sp_search* search, void* userdata){
    qDebug() << "Spotify: search_complete callback called";
    if(sp_search_error(search) == SP_ERROR_OK){
        QtSpotifyWrapper *searchWrapper = (QtSpotifyWrapper *)userdata;
        qDebug() << "Spotify:  You have searched for " << sp_search_query(search);
        searchWrapper->search_complete_wrapper_cb();

    }
    else{
        qDebug() << "Spotify: search result error - " << sp_search_error(search);
    }
    sp_search_release(search);
}

static sp_session_callbacks callbacks = {
    &logged_in,
    &logged_out,
    NULL,
    &connection_error,
    NULL,
    &notify_main_thread,
    NULL,
    NULL,
    &log_message
};

/*!
 * \brief QtSpotifyWrapper::run  This is a reimplementation of the "run()" function
 * in the QThread class.  Within this function is the loop that will call a function
 * which processes any waiting libSpotify calls.  It will run about once every
 * second and will persist through the lifetime of the program.
 */
void QtSpotifyWrapper::run()
{
    sp_error process_error;
    stop = false;
    int next_timeout = 0;

    while(!stop){

        process_error = sp_session_process_events(g_session, &next_timeout);
        msleep(1000);
    }
}

/*!
 * \brief QtSpotifyWrapper::initSpotify  This function sets up all the variables,
 * callbacks, memory allocations, and settings that spotify needs to create a
 * session.  A session represents this computers implementation of libSpotify.
 * This function will also log in to spotify, calling the "logged_in" callback.
 * \param username  The username that the user entered into the settings window of the UI
 * \param password  The password that the user entered into the settings window of the UI
 * \return  a boolean which represents the success (true) or failure (false) of the function
 */
bool QtSpotifyWrapper::initSpotify(QString username, QString password)
{
    sp_session_config config;
    memset(&config, 0, sizeof(config));

    sp_error error;
    sp_error login_error;

    const char* strUsername;
    const char* strPassword;

    qDebug() << "AppKey:" << g_appkey;

    config.api_version=SPOTIFY_API_VERSION;

    config.cache_location= "/var/tmp/Alluvial/alpha";
    config.settings_location="/var/tmp/Alluvial/settings";
    config.application_key = g_appkey;
    qDebug() << g_appkey;
    config.application_key_size = sizeof(g_appkey);
    qDebug() << sizeof(g_appkey);
    config.user_agent = USER_AGENT;
    config.callbacks = &callbacks;
    config.proxy = 0x0;
    config.proxy_password=0x0;
    config.proxy_username=0x0;

    error = sp_session_create(&config, &session);
    if(SP_ERROR_OK != error) {
        qDebug() << "failed to create session: " << sp_error_message(error);
        return 2;
    } else if(SP_ERROR_OK == error) {
        qDebug() << "session created";
    }
    g_session = session;
    /*NEVER TOUCH THIS CODE OR A LARGE PERSON WILL COME TO YOUR
     * HOUSE AND BREAK YOUR COMPUTER IN THE SAME WAY YOU BROKE THIS CODE
     * */
    strUsername = username.toUtf8().constData();
    std::string thing1 = username.toStdString();
    std::string thing2 = password.toStdString();

    strUsername = thing1.c_str();
    strPassword = thing2.c_str();

    qDebug() << strUsername;
    qDebug() << strPassword;

    login_error = sp_session_login(session, strUsername, strPassword, 0, NULL);
    if(SP_ERROR_OK != login_error){
        qDebug() << "failed to login" << sp_error_message(login_error);
        return 0;
    } else if(SP_ERROR_OK == login_error){
        qDebug() << "Login Successful";
    }
    g_session = session;

    start();
}


/*!
 * \brief QtSpotifyWrapper::searchSpotify  The function that is called from the
 * QtSpotifySession::startSearch(query) function in QtSpotifySession class.  It
 * begins the process of searching for media with libspotify
 * \param query  The string that the user types in the search bar. It is the song
 *  which the user wants results for.
 * \return a boolean which represents a true(pass) or false(fail) of the search
 * create
 */
bool QtSpotifyWrapper::searchSpotify(QString query)
{
    qDebug() << "Spotify: searchSpotify request";
    const char * stdQuery = query.toUtf8().constData();
    std::string tmp_string = query.toStdString();
    stdQuery = tmp_string.c_str();
    qDebug() << stdQuery;


    //const char* strQuery = convertToString(searchQuery);
    qDebug() << "searching for - " << stdQuery;
    g_search = sp_search_create(g_session, stdQuery, 0, 10, 0, 10, 0, 10, 0, 10,
                            SP_SEARCH_STANDARD, &search_complete, this);
}

/*!
 * \brief QtSpotifyWrapper::play  The function which loads and starts the playback
 * of a track.  This is to be used when the client sends a request to the server
 * for audio data of a track.
 * \param play  This boolean is used to specify whether playback should stop or
 * start.  True = 1 will play the track specified. False = 0 will pause the track
 * specified
 * \param linkToSong  This QString value is the hash value representing the
 * sp_link object that was passed to the client upon a search request.  It will
 * be used to recreate the track object associated with the search result selected
 * by the user in the client
 * \return a boolean corresponding to the success of the function to play or pause
 * a track
 */
bool QtSpotifyWrapper::play(bool play, QString linkToSong)
{
    const char * stdLink = linkToSong.toUtf8().constData();
    std::string tmp_string = linkToSong.toStdString();
    stdLink = tmp_string.c_str();
    qDebug() << "Spotify: request to play track with link - " << stdLink;

    sp_link *track_link = sp_link_create_from_string(stdLink);

    sp_track * track_to_play = sp_link_as_track(track_link);
    sp_error trackLoadError = sp_session_player_load(g_session, track_to_play);

    while(!sp_track_is_loaded(track_to_play)){
        qDebug() << "Spotify Audio Output: waiting for track to load";
        sleep(0.5);
    }
    if(trackLoadError != SP_ERROR_OK){
        qDebug() << "Spotify Audio Output:  ERROR - " << sp_error_message(trackLoadError);
        return 0;
    } else {
        sp_error spotifyPlayBackError = sp_session_player_play(g_session, play);
    }
    return 1;

}
/*!
 * \brief QtSpotifyWrapper::seek  This function will be used to fetch the audio playback
 * at the new time specified by the user's seek action.
 * \param pos  The new position of the song to begin playback from
 * \return A boolean representing the success or failure of the seek action
 */
bool QtSpotifyWrapper::seek(int pos)
{
    //TODO: implement once playback works properly
}
/*!
 * \brief QtSpotifyWrapper::search_complete_wrapper_cb  The function called when the
 * search callback has been called from Spotify.  This function is used to handle the
 * creation and formatting of the QJsonArray* that represents all of the tracks.
 * This QJsonArray will be sent to the SearchResult class via the
 * searchComplete(QJsonArray *) signal.
 */
void QtSpotifyWrapper::search_complete_wrapper_cb()
{
    //format and parse the tracks given!!!!!!
    QJsonArray *resultsFormatted = new QJsonArray();
    qDebug() << "QtSpotifyWrapper:  reached the search_complete_wrapper_cb";
    qDebug() << "Currently this does nothing except output the results of search";
    for(int i =0; i < sp_search_num_tracks(g_search); i++){
        qDebug() << "Spotify: Search Results - index " << i
                 << "  -  " << sp_track_name(sp_search_track(g_search, i));
        //spotifyTrackList.append(sp_search_track(g_search, i));
        QJsonValue resultsMediaValue = spotifyTrackParser(sp_search_track(g_search, i), i).toObject();
        resultsFormatted->append(resultsMediaValue);
    }
    //Send the results of the search back to the mediahandler
    emit searchComplete(resultsFormatted);
}
