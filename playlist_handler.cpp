#include "playlist_handler.h"
#include "playlist.h"
#include "dataobject.h"
#include <vector>
#include <QDebug>
#include <QAbstractListModel>
#include <QQmlContext>
#include <QQuickView>
#include <math.h>
#include <QFile>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QDir>
#include <QQmlComponent>
#include <QByteArray>

/*!
 * \brief playlist_handler::playlist_handler The wrapper for all playlists to be saved.
 * The playlist wrapper will save indices for which playlist being used and
 * an index for which song is being played in the currently active playlist.
 *
 * Two boolean indicators will be used to save playback options.
 */
playlist_handler::playlist_handler()
{
    this->playlists = std::vector<playlist>();
    this->activePlaylist = 0;
    this->activeSong = 0;
    this->shuffle = false;
    this->repeat = false;
    player = new QMediaPlayer();
    this->player->setNotifyInterval(1000);
    QObject::connect(this->player,SIGNAL(positionChanged(qint64)),
        this, SLOT(childPositionChanged(qint64)));
    QObject::connect(this->player,SIGNAL(durationChanged(qint64)),
        this, SLOT(childDurationChanged()));
}

playlist_handler::~playlist_handler()
{

}

/*!
 * \brief playlist_handler::addPlaylist Add a new playlist to the beginning of the list. Default titled.
 */
void playlist_handler::addPlaylist()
{
    playlist *newPlaylist = new playlist();
    this->playlists.insert(this->playlists.end(), *newPlaylist);

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

/*!
 * \brief playlist_handler::addPlaylist Add a playlist to the beginning of the list
 * \param newPlaylist The playlist to be added
 */
void playlist_handler::addPlaylist(playlist newPlaylist)
{
    this->playlists.insert(this->playlists.end(), newPlaylist);

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

/*!
 * \brief playlist_handler::addPlaylist Add a playlist to the beginning of the list
 * \param playlistTitle The title of the playlist to be added.
 */
void playlist_handler::addPlaylist(QString playlistTitle)
{
    playlist *newPlaylist = new playlist(playlistTitle);
    this->playlists.insert(this->playlists.end(), *newPlaylist);

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

void playlist_handler::addSong(int playlistIndex, playlist_item songToAdd)
{
    this->playlists.at(playlistIndex).addSong(songToAdd);
    if ( playlistIndex == this->activePlaylist )
    {
        emit changeTrackListings(playlistIndex);
    }
}

/*!
 * \brief playlist_handler::dropPlaylists Remove all playlists from the list
 */
void playlist_handler::dropPlaylists()
{
    this->playlists.erase(this->playlists.begin(),this->playlists.end());

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

/*!
 * \brief playlist_handler::dropPlaylist Remove a playlist from the list
 * \param playlistToDrop The playlist item to be dropped.
 */
void playlist_handler::dropPlaylist(playlist playlistToDrop)
{
    for ( int index = 0 ; index < this->playlists.size() ; index++ )
    {
        if ( this->playlists[index].getPlaylistTitle() == playlistToDrop.getPlaylistTitle())
        {
            this->playlists.erase(this->playlists.begin() + index);
            break;
        }
    }

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

/*!
 * \brief playlist_handler::dropPlaylist Remove a playlist from the list
 * \param playlistTitleToDrop The title of the playlist to drop
 */
void playlist_handler::dropPlaylist(QString playlistTitleToDrop)
{
    for ( int index = 0 ; index < this->playlists.size() ; index++ )
    {
        if ( this->playlists[index].getPlaylistTitle() == playlistTitleToDrop)
        {
            this->playlists.erase(this->playlists.begin() + index);
            break;
        }
    }

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
}

void playlist_handler::dropSong(int playlistIndex, playlist_item songToDrop)
{
    this->playlists.at(playlistIndex).removeSong(songToDrop);
}

/*!
 * \brief The playlist incrementer
*/
void playlist_handler::nextSong()
{
    int currentlyActiveSong = this->activeSong;
    if ( this->shuffle )
    {
        while ( this->activeSong == currentlyActiveSong && this->playlists.size() > 1 )
        {
            this->activeSong = rand() % this->playlists.size();
        }
    }
    else
    {
        this->activeSong = this->activeSong + 1;
        if ( this->activeSong >= this->playlists.at(this->activePlaylist).getSongs().size() )
        {
            this->activeSong = 0;
        }
    }

    qDebug() << "Active Song Index:" << this->activeSong;
}

/*!
 * \brief The playlist decrementer
 */
void playlist_handler::previousSong()
{
    int currentlyActiveSong = this->activeSong;
    if ( this->shuffle )
    {
        while ( this->activeSong == currentlyActiveSong && this->playlists.size() > 1 )
        {
            this->activeSong = rand() % this->playlists.size();
        }
    }
    else
    {
        this->activeSong = this->activeSong - 1;
        if ( this->activeSong < 0 )
        {
            this->activeSong = this->playlists.at(activePlaylist).getSongs().size() - 1;
        }
    }

    qDebug() << "Active Song Index:" << this->activeSong;
}

/*!
 * \brief playlist::jumpToSong Jump to song by hash
 * \param newSongHash The hash of the song we want to jump to
 */
void playlist_handler::jumpToSong(QString newSongHash)
{
    for ( int index = 0; index < this->playlists.at(this->activePlaylist).getSongs().size() ; index++ )
    {
        if ( this->playlists.at(this->activePlaylist).getSongs()[index].getHash() == newSongHash )
        {
            this->activeSong = index;
            break;
        }
    }
}

/*!
 * \brief playlist::jumpToSong Skip to song position
 * \param newIndex The index to jump to
 */
void playlist_handler::jumpToSong(int newIndex)
{
    this->activeSong = newIndex;
}

/*!
 * \brief playlist_handler::getPlaylists Get all of the playlists in the wrapper
 * \return A vector containing all playlists item
 */
std::vector<playlist> playlist_handler::getPlaylists()
{
    return this->playlists;
}

/*!
 * \brief playlist_handler::getPlaylist Get the playlist from the specified index
 * \return A playlist item
 */
playlist playlist_handler::getPlaylist(int index)
{
    return this->playlists[index];
}

/*!
 * \brief playlist_handler::getPlaylistNames Get a list containing all playlist names
 * \return A vector containing playlist names
 */
std::vector<QString> playlist_handler::getPlaylistNames()
{
    std::vector<QString> titles;
    for ( int index = 0 ; index < this->playlists.size() ; index++ )
    {
        titles.insert(titles.begin() + index, this->playlists[index].getPlaylistTitle());
    }
    return titles;
}

/*!
 * \brief playlist_handler::getPlaylistSongNames Get a vector containing the songs in the current playlist
 * \param playlistIndex The index of the playlist to get songs from
 * \return A vector containing song names
 */
std::vector<QString> playlist_handler::getPlaylistSongNames(int playlistIndex)
{
    playlist toCheck = this->playlists.at(playlistIndex);
    std::vector<QString> titles;
    for ( int index = 0 ; index < toCheck.getSongs().size() ; index++ )
    {
        titles.insert(titles.begin() + index, toCheck.getSongs()[index].getSongName());
    }
    return titles;
}

/*!
 * \brief playlist_handler::getActivePlaylist Get the current playlist being used
 * \return The active playlist
 */
playlist playlist_handler::getActivePlaylist()
{
    return this->playlists[this->activePlaylist];
}

/*!
 * \brief playlist_handler::getActivePlaylistIndex Get the index of the active playlist
 * \return An int indicating which playlist to use
 */
int playlist_handler::getActivePlaylistIndex()
{
    return this->activePlaylist;
}

/*!
 * \brief Get the hash of the song to be played.
 */
QString playlist_handler::getActiveSongHash()
{
    return this->playlists.at(this->activePlaylist).getSongs()[this->activeSong].getHash();
}

/*!
 * \brief playlist_handler::getShuffle Get the state of the shuffle function
 * \return A boolean indicator for the shuffle value
 */
bool playlist_handler::getShuffle()
{
    return this->shuffle;
}

/*!
 * \brief playlist_handler::getRepeat Get the state of the repeat function
 * \return A boolean indicator for the repeat value
 */
bool playlist_handler::getRepeat()
{
    return this->repeat;
}

/*!
 * \brief playlist_handler::setActivePlaylistIndex Change which playlist is playing
 * \param newIndex The index of the playlist to be used
 */
void playlist_handler::setActivePlaylistIndex(int newIndex)
{
    this->activePlaylist = newIndex;
}

/*!
 * \brief playlist_handler::setActiveSongIndex Change which song is playing
 * \param newIndex The index of the song to be set
 */
void playlist_handler::setActiveSongIndex(int newIndex)
{
    this->activeSong = newIndex;
}

/*!
 * \brief playlist_handler::setShuffle Set the shuffle function value
 * \param shuf A boolean indicator for the new set
 */
void playlist_handler::setShuffle(bool shuf)
{
    this->shuffle = shuf;
}

/*!
 * \brief playlist_handler::setRepeat Set the repeat function value
 * \param rep A boolean indicator for the new set
 */
void playlist_handler::setRepeat(bool rep)
{
    this->repeat = rep;
}

/*!
 * \brief playlist_handler::repeatSwitch Toggle the state of the repeat function
 */
void playlist_handler::repeatSwitch()
{
    if ( this->repeat )
    {
        this->repeat = false;
    }
    else
    {
        this->repeat = true;
    }
    qDebug() << "Repeat has been switched:" << this->repeat;
}

/*!
 * \brief playlist_handler::shuffleSwitch Toggle the state of the shuffle function
 */
void playlist_handler::shuffleSwitch()
{
    if (this->shuffle )
    {
        this->shuffle = false;
    }
    else
    {
        this->shuffle = true;
    }
    qDebug() << "Shuffle has been switched:" << this->shuffle;
}

/*!
 * \brief playlist_handler::changePlaylist Switch the active playlist to the specified playlist.
 * \param newIndex The index of the playlist to switch to
 */
void playlist_handler::changePlaylist(int newIndex)
{
    this->setActivePlaylistIndex(newIndex);
    this->setActiveSongIndex(0);
    qDebug() << "Playlist has been changed:" << this->getActivePlaylist().getPlaylistTitle();
}

/*!
 * \brief playlist_handler::changePlaylist Switch the active playlist to the specified playlist.
 * \param playlistTitle The title of the playlist to switch to.
 */
void playlist_handler::changePlaylist(QString playlistTitle)
{
    for ( int index = 0 ; index < this->playlists.size() ; index++ )
    {
        if ( this->playlists[index].getPlaylistTitle() == playlistTitle)
        {
            this->changePlaylist(index);
            break;
        }
    }
}

/*!
 * \brief playlist_handler::changeTrackListings Changes to the indexed playlist and displays the proper info
 * \param index The index of the playlist changed to
 */
void playlist_handler::changeTrackListings(int index)
{
    this->changePlaylist(index);

    QStringList dataList;
    for ( int i = 0; i < this->getActivePlaylist().getSongs().size() ; i++ )
    {
        dataList.append(this->getActivePlaylist().getSongs().at(i).getSongName());
    }

    emit setTrackListings(QVariant::fromValue(dataList));
}

// Taken from old mediaplayer class
/*!
 * \brief ::play Continue the song to play
 */
void playlist_handler::play ()
{
    player->play();
}

/*!
 * \brief playlist_handler::play Play a brand new song
 * \param data The data of the song to play
 */
void playlist_handler::play (QByteArray data)
{
    QFile tmp_file("tmp.mp3");
    QDir path = QDir::currentPath();

    if(!tmp_file.open(QIODevice::WriteOnly)) {
        qDebug() << "opening location didn't work";
        return ;
    }

    if(tmp_file.write(data) < 0) {
        qDebug() << "file write failed" << tmp_file.errorString();
        return ;
    }
    tmp_file.close();

    QMediaContent song = QMediaContent(QUrl::fromLocalFile(path.absolutePath() + "/tmp.mp3"));
    player->setMedia(song);
    player->setVolume(50);
    player->play();
}

/*!
 * \brief playlist_handler::pause Pause the current song
 */
void playlist_handler::pause ()
{
    player->pause();
}

/*!
 * \brief playlist_handler::playOrPause Switch the playback state
 */
void playlist_handler::playOrPause()
{
    int state = player->state();

    if (state == 1)
    {
        this->pause();
    }
    else
    {
        this->play();
    }
}

/*!
 * \brief playlist_handler::skipTo Jump to a specific position in the song
 * \param position The position to jump to in milliseconds
 */
void playlist_handler::skipTo(int position)
{
    // position in milliseconds
    player->setPosition(position);
}

/*!
 * \brief playlist_handler::setVolume Change the playback volume
 * \param vol The value to change to on a scale of 1 - 100
 */
void playlist_handler::setVolume(int vol)
{
    player->setVolume(vol);
}

/*!
 * \brief playlist_handler::startFastForward
 * Increase the playback rate to skip ahead through the song.
 * Also, sets the display to update every fraction of a second instead of a whole second.
 */
void playlist_handler::startFastForward()
{
    player->setVolume(player->volume() / 3);
    player->setPlaybackRate(10);
    this->player->setNotifyInterval(1000/10);
    qDebug() << "Playback rate =" << player->playbackRate();
    qDebug() << this->player->position();
}

/*!
 * \brief playlist_handler::startRewind
 * Set the playback rate to go backwards through a song.
 * Also, sets the display to update every fraction of a second instead of a whole second.
 *
 * TODO: Currently broken for some unknown reason. Always skips to start of song instead of rewinding
 */
void playlist_handler::startRewind()
{
    player->setVolume(player->volume() / 3);
    this->player->setPlaybackRate(-6);
    this->player->setNotifyInterval(1000/6);
    qDebug() << "Playback rate =" << player->playbackRate();
    qDebug() << this->player->position();
}

/*!
 * \brief playlist_handler::resetPlaybackRate Resumes the song at the normal playback rate.
 * Also, adjusts the re-display notifier back to a whole second
 * instead of wasting cycles every fraction of a second.
 */
void playlist_handler::resetPlaybackRate()
{
    qDebug() << this->player->position();
    player->setVolume(player->volume() * 3);
    player->play();
    player->setPlaybackRate(1);
    this->player->setNotifyInterval(1000);
    qDebug() << "Playback rate reset";
    qDebug() << this->player->position();
}

/*!
 * \brief playlist_handler::childPositionChanged Emits a signal when the song position has changed.
 * Only called when the notifier checks the value.
 * \param position The position of the player in the song.
 */
void playlist_handler::childPositionChanged(qint64 position)
{
    emit positionChanged(position);
}

/*!
 * \brief playlist_handler::childDurationChanged Emits a signal when the song duration is loaded.
 * This signal slots to the Playback Bar to fix the song end label.
 */
void playlist_handler::childDurationChanged()
{
    int length = this->player->duration();
    length = length / 1000;
    qDebug() << "playlist_handler::Duration change called:" << this->player->duration();
    emit durationChanged(QVariant(length));
}
