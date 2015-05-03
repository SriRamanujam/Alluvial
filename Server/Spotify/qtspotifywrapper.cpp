#define USER_AGENT "AlluvialMusicPlayer1"

#include "qtspotifywrapper.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <chrono>
#include <sys/time.h>
#include <pthread.h>
#include "../../appkey.h"
#include <QMutex>

int timeout;
static sp_session *session;

//Synchronization variable to tell main thread ot process spotify events
static int notify_events;
static pthread_mutex_t notify_mutex;
static pthread_cond_t notify_cond;

QtSpotifyWrapper::QtSpotifyWrapper()
{
    FLAG_SPOTIFY_READY = false;
    timeout = -1;
    //sp_session *sessionObj;
}

QtSpotifyWrapper::~QtSpotifyWrapper()
{
    sp_session_release(g_session);

}

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

        artistString.append(sp_artist_name(tmpArtist));
        artistString.append(", ");
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

static void  connection_error(sp_session *session, sp_error error){
    qDebug() << "Spotify:  Connection Error - " << sp_error_message(error) << " - for user - " << sp_session_user(session);
}

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

static void logged_out(sp_session *session){
    qDebug() << "Spotify: Logged Out for user - " << sp_session_user(session);
}

static void log_message(sp_session *session, const char *data){
    qDebug() << "Spotify: log_message for user " << sp_session_user(session) << " - "  << data;
}


static void notify_main_thread(sp_session *session){
    qDebug() << "notify_main_thread callback received";
    pthread_mutex_lock(&notify_mutex);
    notify_events = 1;
    pthread_cond_signal(&notify_cond);
    pthread_mutex_unlock(&notify_mutex);
}


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

void QtSpotifyWrapper::run()
{
    sp_error process_error;
    stop = false;
    int next_timeout = 0;
    QMutex processEventMutex;


    while(!stop){

        //sleep(3);

        process_error = sp_session_process_events(g_session, &next_timeout);
        //usleep(1000);

    }

/*
    pthread_mutex_init(&notify_mutex, NULL);
    pthread_cond_init(&notify_cond, NULL);

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
            ts.tv_nsec += (next_timeout%1000) * 1000;

            while(!notify_events){
                if(!pthread_cond_timedwait(&notify_cond, &notify_mutex, &ts)){
                    break;
                }
            }
        }
        //process lib spotify events apparently...
        notify_events = 0;
        pthread_mutex_unlock(&notify_mutex);

        do{
            process_error = sp_session_process_events(g_session, &next_timeout);
        }while(next_timeout==0);
        pthread_mutex_lock(&notify_mutex);
    }
    */
}


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

    start();
}

void QtSpotifyWrapper::login()
{

}

void QtSpotifyWrapper::logout()
{

}

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

bool QtSpotifyWrapper::play(bool play)
{

}

bool QtSpotifyWrapper::seek(int pos)
{

}

void QtSpotifyWrapper::search_complete_wrapper_cb()
{
    //format and parse the tracks given!!!!!!
    QList <sp_track *> spotifyTrackList;
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
