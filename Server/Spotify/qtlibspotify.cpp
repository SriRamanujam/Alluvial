
/******************************************************

  STOP HERE!  NO SERIOUSLY FUCKING STOP RIGHT NOW!
  IN ORDER FOR THIS CODE TO WORK YOU MUST GET A LIBSPOTIFY
  APPLICATION KEY FROM: https://devaccount.spotify.com/my-account/keys/

  GOT IT, YET?????????? PROBABLY NOT BUT FUCK IT!!!
  OK SO NOW WHEN YOU HAVE YOUR APPLICATION KEY, TAKE IT AND PLACE
  IT IN THE PARENT DIRECTORY OF THE FOLDER WHICH THIS FILE
  (fyi: this file is called qtlibspotify.cpp) IS CURRENTLY!

  DO NOT PUT IT ANYWHERE ELSE!! IF YOU DO A BAD PERSON WILL FIND YOU
  EAT ALL OF YOUR NUTELLA AND PEANUT BUTTER AND ICE CREAM. HE WILL
  ALSO CLOG YOUR TOILETS WITH YOU FAVORITE PAIR OF SOCKS!

  *****************************************************/

#define USER_AGENT "AlluvialMusicPlayer1"

#include "qtlibspotify.h"
#include <QDebug>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <string.h>

#include "../../appkey.h"

sp_session *g_session;

//Concurrency variables
static int notify_events;       //Synchronization variable to tell main thread ot process spotify events
static pthread_mutex_t notify_mutex;
static pthread_cond_t notify_cond;

bool FLAG_SPOTIFY_STATUS;

sp_search *res;

static void  connection_error(sp_session *session, sp_error error){
    qDebug() << "Spotify:  Connection Error - " << sp_error_message(error);
}

static void logged_in(sp_session *session, sp_error error){
    qDebug() << "Spotify: Logged_in Reached";
    if(SP_ERROR_OK != error){
        FLAG_SPOTIFY_STATUS=false;
        qDebug() << "Spotify:  Log in failed: " << sp_error_message(error);
    }
    else{
        qDebug() << "Spotify: Log in successful";
        FLAG_SPOTIFY_STATUS=true;
    }

}

static void logged_out(sp_session *session){
    qDebug() << "Spotify: Logged Out";
}

static void log_message(sp_session *session, const char *data){
    qDebug() << "Spotify: log_message - "<< data;
}

static void notify_main_thread(sp_session *session){
    qDebug() << "notify_main_thread callback received";
    pthread_mutex_lock(&notify_mutex);
    notify_events = 1;
    pthread_cond_signal(&notify_cond);
    pthread_mutex_unlock(&notify_mutex);
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

 const char* convertToString(QString strInput){
    QByteArray byteForm = strInput.toLatin1();
    return byteForm.data();
    //qDebug() << "Converted [" << strInput <<"] to [" << charPtrForm << "]";
    //return &charPtrForm;
}

static void SP_CALLCONV search_complete(sp_search* search, void* userdata){
    qDebug() << "Spotify: search_complete callback called";
    if(sp_search_error(search) == SP_ERROR_OK){



        //format and return the search results?????
        /*
         * options:
         *  1) return three result type for albums, artist, and tracks
         *
         *  2) return tracks with info about album/artist/etc
         * */
    }
    else{
        qDebug() << "Spotify: search result error - " << sp_search_error(search);
    }
    sp_search_release(search);
}
/*
static getDevice_ID(){
    FILE file;
}
*/

sp_connectionstate QtLibSpotify::GetConnectionState(){
    if(g_session)
    {
        sp_connectionstate connectState = sp_session_connectionstate(g_session);
        return connectState;
    }
    else
    {
        return SP_CONNECTION_STATE_LOGGED_OUT;
    }
}

QtLibSpotify::QtLibSpotify(QString username, QString password)
{
    int next_timeout = 0;

    pthread_mutex_init(&notify_mutex, NULL);
    pthread_cond_init(&notify_cond, NULL);

    if(initSpotify(username, password) != 0){
        qDebug() << "Spotify failed to initialized";
        exit(-1);
    }
    pthread_mutex_lock(&notify_mutex);
    for(;;){
        if(next_timeout ==0){
               while(!notify_events){
                   pthread_cond_wait(&notify_cond, &notify_mutex);
               }
        } else {
            struct timespec ts;
#if _POSIX_TIMERS >0
            clock_gettime(CLOCK_REALTIME, &ts);
#else
            struct timeval tv;
            gettimeofday(&tv, NULL);
            TIMEVAL_TO_TIMESPEC(&tv,&ts);
#endif
            ts.tv_sec += next_timeout/1000;
            ts.tv_nsec += (next_timeout%1000) * 1000000;

            while(!notify_events){
                if(pthread_cond_timedwait(&notify_cond,&notify_mutex,&ts)){
                    break;
                }
            }
        }
        //process lib spotify events apparently...
        notify_events = 0;
        pthread_mutex_unlock(&notify_mutex);

        do{
            sp_session_process_events(g_session, &next_timeout);
        }while(next_timeout==0);
        pthread_mutex_lock(&notify_mutex);
    }
}

QtLibSpotify::~QtLibSpotify()
{

}

int QtLibSpotify::initSpotify(QString username, QString password){

    sp_session_config config;
    memset(&config, 0, sizeof(config));
    sp_session *session;
    sp_error error;
    sp_error login_error;


    const char* strUsername;
    const char* strPassword;

    qDebug() << "AppKey:" << g_appkey;

    config.api_version=SPOTIFY_API_VERSION;

    config.cache_location= "tmp";
    config.settings_location="tmp";
    config.application_key = g_appkey;
    qDebug() << g_appkey;
    config.application_key_size = sizeof(g_appkey);
    qDebug() << sizeof(g_appkey);
    config.user_agent = USER_AGENT;
    config.callbacks = &callbacks;
    //FUCK PROXIES
    config.proxy = 0x0;
    config.proxy_password=0x0;
    config.proxy_username=0x0;
   // config.device_id= getDevice_ID();

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
        return 3;
    } else if(SP_ERROR_OK == login_error){
        qDebug() << "Login Successful";
    }
    g_session = session;

    return 0;
}

sp_search *QtLibSpotify::searchSpotify(sp_session *session, QString searchQuery){
    qDebug() << "Spotify: searchSpotify request";

    const char* strQuery = convertToString(searchQuery);
    qDebug() << "searching for - " << strQuery;
    return sp_search_create(session, strQuery, 0, 10, 0, 10, 0, 10, 0, 10, SP_SEARCH_STANDARD, &search_complete, NULL);
}

void QtLibSpotify::playSongSpotify(sp_session *session, sp_track* track){
    sp_error error = sp_session_player_load(session, track);
    if(SP_ERROR_OK != error){
        qDebug() << "Spotify: Loading " <<sp_error_message(error);
    }
    else{
        sp_session_player_play(session, 1);
    }
}

bool QtLibSpotify::isLoggedIn(){
    bool isloggedin = (NULL != g_session) && (GetConnectionState());
}

/*
 * Public Slots
 *
 */
sp_error QtLibSpotify::spotifyLogin(QString username, QString password){
    qDebug() << "spotifyLogin Signal was picked up";
    sp_error spotifyLoginError;
    const char* strUsername = convertToString(username);
    const char* strPassword = convertToString(password);
    //qDebug() << "Spotify: try relogin - " << sp_session_relogin(user_session);
    spotifyLoginError = sp_session_login(g_session, strUsername, strPassword, 1, NULL);
    qDebug() <<"Spotify: try login - " << sp_error_message(spotifyLoginError);
    return spotifyLoginError;
}

sp_error QtLibSpotify::spotifyLogout(sp_session *user_session){
    qDebug() << "spotifyLogout Signal was picked up";
    return sp_session_logout(user_session);
}

sp_error QtLibSpotify::search(QString searchString){
    res = searchSpotify(g_session, searchString);
    qDebug() << "Spotify search: Number of search results - " << sp_search_total_tracks(res);
}

sp_error QtLibSpotify::releaseSpotifySession(sp_session *user_session){
    sp_error release_error;
    release_error = sp_session_release(user_session);
    qDebug() << "Spotify: TRY releasing session - " << sp_error_message(release_error);
}

sp_error QtLibSpotify::closing(){
    emit releaseSpotifySession(g_session);
}

