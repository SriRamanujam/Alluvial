#include "playlist_handler.h"
#include "playlist.h"
#include "dataobject.h"
#include <string>
#include <vector>
#include <QDebug>
#include <QJsonDocument>
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
    this->player = new QMediaPlayer();
    this->player->setVolume(50);
    this->player->setNotifyInterval(1000);

    this->timer = new QTime();
    this->rewindTimer = new QTimer(this);
    this->rewindTimer->setSingleShot(false);

    this->ffTimer = new QTimer(this);
    this->ffTimer->setSingleShot(false);

    QObject::connect(this->player,SIGNAL(positionChanged(qint64)),
        this, SLOT(childPositionChanged(qint64)));
    QObject::connect(this->player,SIGNAL(durationChanged(qint64)),
        this, SLOT(childDurationChanged()));
    QObject::connect(this->player, SIGNAL(metaDataChanged()),
        this, SLOT(metaDataChanged()));
    //QObject::connect(this->player, SIGNAL(metaDataChanged(QString, QVariant)),
    //    this, SLOT(metaDataChanged(QString, QVariant)));
    QObject::connect(this->rewindTimer, SIGNAL(timeout()),
        this, SLOT(updatePositionWhileRewinding()));
    QObject::connect(this->ffTimer, SIGNAL(timeout()),
        this, SLOT(updatePositionWhileFastForwarding()));
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

    this->dropPlaylist("Empty Playlist");

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

    this->dropPlaylist("Empty Playlist");

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
    this->dropPlaylist("Empty Playlist");

    playlist *newPlaylist = new playlist(playlistTitle);
    this->playlists.insert(this->playlists.end(), *newPlaylist);

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
    this->activePlaylist = this->playlists.size() - 1;
    this->changeTrackListings(this->getActivePlaylistIndex());
}

/*!
 * \brief playlist_handler::addPlaylist A slot function which creates a new playlist based off user input
 * \param playlistTitle A QVariant containing the title of the playlist to be created
 */
void playlist_handler::addPlaylist(QVariant playlistTitle)
{
    this->addPlaylist(playlistTitle.toString());
}

/*!
 * \brief playlist_handler::addSong Add a song to the end of the active playlist based off of the search results
 * \param name The song title
 * \param hash The song lookup hash for playback
 * \param artist The name of the artist of the song
 * \param album The album this song was included in, if any
 * \param length The length of the song in seconds
 * \param genre A list of genres, seperated by commas
 * \param trackNumber The number on the album that this track was a part of
 */
void playlist_handler::addSong(QVariant name, QVariant hash, QVariant artist, QVariant album,
        int length, QVariant genre, int trackNumber)
{
    playlist_item *toInsert = new playlist_item(hash.toString(), name.toString(), length,
                album.toString(), artist.toString(), trackNumber, genre.toString());
    this->playlists.at(activePlaylist).addSong(*toInsert);
    this->playlists.at(activePlaylist).setTime();

    if ( this->getActivePlaylist().getSongs().size() == 1)
    {
        this->jumpToSong(0);
    }

    this->changeTrackListings(this->activePlaylist);
    this->savePlaylist(this->activePlaylist);
}

/*!
 * \brief playlist_handler::addSong Adds a new song to a specific playlist
 * \param playlistIndex The playlist to be added to
 * \param songToAdd A playlist_item containing all of the song metadata
 */
void playlist_handler::addSong(int playlistIndex, playlist_item songToAdd)
{
    this->playlists.at(playlistIndex).addSong(songToAdd);
    if ( playlistIndex == this->activePlaylist )
    {
        if ( this->getActivePlaylist().getSongs().size() == 1)
        {
            this->jumpToSong(0);
        }
        this->changeTrackListings(playlistIndex);
    }
    this->playlists.at(activePlaylist).setTime();
    this->savePlaylist(playlistIndex);
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
            if ( index > 0)
            {
                this->changePlaylist(this->activePlaylist-1);
            }
            else
            {
                this->changePlaylist(0);
            }
            break;
        }
    }

    QStringList dataList;
    for ( int i = 0; i < this->getPlaylists().size() ; i++ )
    {
        dataList.append(this->getPlaylists().at(i).getPlaylistTitle());
    }

    emit setPlaylistListings(QVariant::fromValue(dataList));
    this->changePlaylist(this->activePlaylist);
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
            if ( index > 0)
            {
                this->changePlaylist(this->activePlaylist-1);
            }
            else
            {
                this->changePlaylist(0);
            }
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
 * \brief playlist_handler::dropSong Delete a song from the active playlist from the given index
 * \param songToDrop The index of the song in the vector of the playlists' songs
 */
void playlist_handler::dropSong(int songToDrop)
{
    if (this->activeSong >= songToDrop)
    {
        this->activeSong--;
    }

    if (this->getActivePlaylist().getSongs().length() > 1)
    {
        if (this->activePlaylist == this->playlists.size()-1)
        {
            this->activePlaylist--;
        }
        this->playlists.at(this->activePlaylist).removeSong(songToDrop);
        this->changeTrackListings(this->activePlaylist);
        this->playlists.at(activePlaylist).setTime();
        this->savePlaylist(this->activePlaylist);
    }
    else if ( this->playlists.size() > 1 )
    {
        if (this->activePlaylist == this->playlists.size()-1)
        {
            this->activePlaylist--;
        }
        this->dropPlaylist(this->getActivePlaylist());
        this->changeTrackListings(this->activePlaylist);
    }
    else
    {
        playlist_item *dummy = new playlist_item();
        this->dropPlaylists();
        this->addPlaylist(QString("Empty Playlist"));
        this->addSong(this->playlists.size()-1, *dummy);
        this->changePlaylist("Empty Playlist");
        this->changeTrackListings(this->activePlaylist);
    }
}

/*!
 * \brief playlist_handler::dropSong Delete a song from a specific playlist
 * \param playlistIndex The playlist the song is in
 * \param songToDrop The playlist_item containing the metadata of the song to remove
 */
void playlist_handler::dropSong(int playlistIndex, playlist_item songToDrop)
{
    this->playlists.at(playlistIndex).removeSong(songToDrop);
    this->playlists.at(activePlaylist).setTime();
    this->savePlaylist(playlistIndex);
}

/*!
 * \brief The playlist incrementer
*/
void playlist_handler::nextSong()
{
    if (this->getActivePlaylist().getPlaylistTitle().compare("Single Song") == 0)
    {
        this->dropPlaylist(this->getActivePlaylist());
    }
    else
    {
        int currentlyActiveSong = this->activeSong;
        if ( this->shuffle )
        {
            while ( this->activeSong == currentlyActiveSong && this->getActivePlaylist().getSongs().size() > 1 )
            {
                this->activeSong = rand() % this->getActivePlaylist().getSongs().size();
            }
            emit changeActiveSong(QVariant(this->activeSong));
            emit loadingSong();
            this->pause();
            emit requestSong(this->getActiveSongHash());
        }
        else
        {
            this->activeSong = this->activeSong + 1;
            if ( this->activeSong >= this->playlists.at(this->activePlaylist).getSongs().size() )
            {
                this->activeSong = 0;
                if ( this->repeat )
                {
                    emit changeActiveSong(QVariant(this->activeSong));
                    emit loadingSong();
                    this->pause();
                    emit requestSong(this->getActiveSongHash());
                }
            }
            else
            {
                emit changeActiveSong(QVariant(this->activeSong));
                emit loadingSong();
                this->pause();
                emit requestSong(this->getActiveSongHash());
                this->pause();
                emit songPaused();
            }
        }


    }
}

/*!
 * \brief The playlist decrementer
 */
void playlist_handler::previousSong()
{
    if ( this->getActivePlaylist().getSongs().size() == 1)
    {
        this->player->setPosition(0);
        this->play();
    }
    else
    {
        int currentlyActiveSong = this->activeSong;
        if ( this->shuffle )
        {
            while ( this->activeSong == currentlyActiveSong && this->getActivePlaylist().getSongs().size() )
            {
                this->activeSong = rand() % this->getActivePlaylist().getSongs().size();
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

        emit changeActiveSong(QVariant(this->activeSong));
        emit loadingSong();
        this->pause();
        emit requestSong(this->getActiveSongHash());
    }
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
    emit loadingSong();
    this->pause();
    emit requestSong(this->getActiveSongHash());
}

/*!
 * \brief playlist::jumpToSong Skip to song position
 * \param newIndex The index to jump to
 */
void playlist_handler::jumpToSong(int newIndex)
{
    this->activeSong = newIndex;
    emit loadingSong();
    this->pause();
    emit requestSong(this->getActiveSongHash());
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
    if (this->getActivePlaylist().getSongs().size() > 0)
    {
        emit loadingSong();
        this->pause();
        emit requestSong(this->getActiveSongHash());
    }
    this->changeTrackListings(this->activePlaylist);
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

    if (this->getActivePlaylist().getSongs().size() > 0)
    {
        emit loadingSong();
        this->pause();
        emit requestSong(this->getActiveSongHash());
    }
    this->changeTrackListings(this->activePlaylist);
}

/*!
 * \brief playlist_handler::changeTrackListings Changes to the indexed playlist and displays the proper info
 * \param index The index of the playlist changed to
 */
void playlist_handler::changeTrackListings(int index)
{
    qDebug() << "Change track listings called" << index;
    if ( index == this->activePlaylist )
    {

    }
    else
    {
        this->changePlaylist(index);
        qDebug() << "changePlaylist called " << index;
    }

    qDebug() << "Number of songs in active playlist:" << this->getActivePlaylist().getSongs().size();

    QStringList dataList;
    for ( int i = 0; i < this->getActivePlaylist().getSongs().size() ; i++ )
    {
        dataList.append(this->getActivePlaylist().getSong(i).getSongName());
    }
    if ( this->getActivePlaylist().getSongs().size() == 0 )
    {
        dataList.append("");
    }
    emit setTrackListings(QVariant::fromValue(dataList));
}

// Taken from old mediaplayer class
/*!
 * \brief play Resume the currently active song
 */
void playlist_handler::play ()
{
    this->player->play();
    emit playSong();
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
    this->player->setMedia(song);
    this->play();
}

/*!
 * \brief playlist_handler::pause Pause the current song
 */
void playlist_handler::pause ()
{
    player->pause();
    emit songPaused();
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
    this->pause();
    this->timer->start();
    this->ffTimer->setInterval(1000/10);
    this->ffTimer->start();
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
    this->pause();
    this->timer->start();
    this->rewindTimer->setInterval(1000/10);
    this->rewindTimer->start();
}

/*!
 * \brief playlist_handler::updatePositionWhileRewinding Decreases the player position by a second and plays a snippet.
 *
 *
 * This function triggers every tenth of a second. When triggered, it plays a brief snippet of the song (by design)
 * and moves the position of the song back by a second. Because of the brief snippet played, occasionally
 * the playback bar won't go back a full second. It is still rewinding despite appearances.
 */
void playlist_handler::updatePositionWhileRewinding()
{
    this->play();
    this->pause();
    this->player->setPosition(this->player->position()-1000);
    this->timer->restart();
}

/*!
 * \brief playlist_handler::updatePositionWhileFastForwarding Advances the player position by a second and plays a snippet.
 *
 *
 * This function triggers every tenth of a second. When triggered, it plays a brief snippet of the song (by design)
 * and moves the position of the song ahead a second. Because of the brief snippet played, occasionally
 * the playback bar will go ahead several seconds. It is still fast forwarding at the normal rate despite appearances.
 */
void playlist_handler::updatePositionWhileFastForwarding()
{
    this->play();
    this->pause();
    this->player->setPosition(this->player->position()+1000);
    this->timer->restart();
}

/*!
 * \brief playlist_handler::resetPlaybackRate Resumes the song at the normal playback rate.
 * Also, adjusts the re-display notifier back to a whole second
 * instead of wasting cycles every fraction of a second.
 */
void playlist_handler::resetPlaybackRate()
{
    this->rewindTimer->stop();
    this->ffTimer->stop();
    this->timer = new QTime();

    this->play();
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

/*!
 * \brief playlist_handler::metaDataChanged Updates display info when the active song's metadata changes
 *
 *
 * If a song has the meta data for the display set (Artist, Album, Title), the song will display this metadata.
 * However, if this info can not be found, the display will grab the info from the saved metadata. This
 * may not be desirable as some times the meta data is obfuscated to be more descriptive.
 * i.e., Song Title - "Migos - Hannah Montana by DBJUU" when the song title is really "Hannah Montana"
 */
void playlist_handler::metaDataChanged()
{
    QStringList metadata;

    if ( this->player->metaData("Artist").isNull() )
    {
        metadata.append(this->getActivePlaylist().getSong(this->activeSong).artist);
    }
    else
    {
        metadata.append(this->player->metaData("Artist").toString());
    }

    if ( this->player->metaData("AlbumTitle").isNull() )
    {
        metadata.append(this->getActivePlaylist().getSong(this->activeSong).album);
    }
    else
    {
        metadata.append(this->player->metaData("AlbumTitle").toString());
    }

    if ( this->player->metaData("Title").isNull() )
    {
        metadata.append(this->getActivePlaylist().getSong(this->activeSong).getSongName());
    }
    else
    {
        metadata.append(this->player->metaData("Title").toString());
    }

    emit displayData(QVariant::fromValue(metadata));
}

/*!
 * \brief playlist_handler::metaDataChanged Log the new metadata discovered in the console.
 *
 *
 * When a song is set as the active song, meta data comes in tiny pieces. As these pieces are discovered,
 * this function will log what types of metadata is available for the song, along with the values.
 *
 * \param key The type of metadata discovered
 * \param value The value of the metadata set.
 */
void playlist_handler::metaDataChanged(QString key, QVariant value)
{
    qDebug() << key << ":" << value.toString();
}

/*!
 * \brief playlist_handler::onSearchReceived Parses search results and gives the parsed data to the QML to display
 *
 *
 * Gets the search results from the communicator as a JSon Object. This function goes through the JSon to get lists
 * of all of the available metadata which will be necessary for playing/saving the music.
 * \param doc The JSon document containing search results
 */
void playlist_handler::onSearchReceived(QJsonObject doc)
{
    QStringList songNames;
    QStringList hashes;
    QStringList artists;
    QStringList albums;
    QStringList strLengths;
    QStringList genres;
    QList<int> lengths;
    QList<int> trackNumbers;

    QJsonArray results = doc["response"].toObject()["results"].toObject()["results"].toArray();

    foreach (QJsonValue result, results)
    {
        hashes.append(result.toObject()["hash"].toString());

        songNames.append(result.toObject()["metadata"].toObject()["title"].toString());
        artists.append(result.toObject()["metadata"].toObject()["artist"].toString());
        albums.append(result.toObject()["metadata"].toObject()["album"].toString());
        strLengths.append(result.toObject()["metadata"].toObject()["length"].toString());
        genres.append(result.toObject()["metadata"].toObject()["genre"].toString());
        trackNumbers.append(result.toObject()["metadata"].toObject()["track_number"].toInt());

        QString strLength = result.toObject()["metadata"].toObject()["length"].toString();
        std::string stdStrLength = strLength.toStdString();
        int mins = 0;
        int secs = 0;
        int index = 0;
        while (strLength[index].isNumber())
        {
            mins = mins * 10 + atoi(stdStrLength.substr(index, 1).c_str());
            index++;
        }
        index++;
        while (strLength[index].isNumber())
        {
            secs = secs * 10 + atoi(stdStrLength.substr(index, 1).c_str());
            index++;
        }
        int length = mins * 60 + secs;
        lengths.append(length);
    }

    emit displaySearchResults(QVariant::fromValue(songNames), QVariant::fromValue(hashes),
        QVariant::fromValue(artists), QVariant::fromValue(albums), QVariant::fromValue(lengths),
        QVariant::fromValue(genres), QVariant::fromValue(trackNumbers), QVariant::fromValue(strLengths));
}

/*!
 * \brief playlist_handler::savePlaylist Creates a JSonObject containing all of the playlist info for saving purposes
 * \param index The index of the playlist to be saved.
 */
void playlist_handler::savePlaylist(int index)
{
    QJsonArray tracks;

    this->getActivePlaylist().setTime();

    QJsonObject playlist {
        {"name", this->getPlaylist(index).getPlaylistTitle()},
        {"last_saved", this->getPlaylist(index).getTime().toString()},
    };

    for ( int i = 0 ; i < this->getPlaylist(index).getSongs().length() ; i++ )
    {
        playlist_item song = this->getPlaylist(index).getSong(i);
        QString title = song.getSongName();
        QString album = song.album;
        QString artist = song.artist;
        QString track_number = QVariant(song.track_number).toString();
        QString length = song.stringLength;
        QString genre = song.genre;

        QJsonObject metadata{
            {"title", title},
            {"album", album},
            {"artist", artist},
            {"track_number", track_number},
            {"length", length},
            {"genre", genre},
        };

        QJsonObject media {
            {"hash", song.getHash()},
            {"order", i},
        };

        media["metadata"] = metadata;
        tracks.append(media);
    }
    playlist["tracks"] = tracks;
}

/*!
 * \brief playlist_handler::getAllPlaylists Parses a QJsonArray containing all of the playlists that have been saved
 * and adds them to the media player.
 * \param playlists A QJsonArray containing all of the playlist info.
 */
void playlist_handler::getAllPlaylists(QJsonArray playlists)
{
    foreach (QJsonValue play, playlists)
    {
        QString title = play.toObject()["name"].toString();
        QJsonArray tracks = play.toObject()["tracks"].toArray();
        playlist *tmp = new playlist(title);
        foreach (QJsonValue track, tracks)
        {
            QString songName = track.toObject()["metadata"].toObject()["title"].toString();
            QString hash = track.toObject()["hash"].toString();
            QString album = track.toObject()["metadata"].toObject()["album"].toString();
            QString artist = track.toObject()["metadata"].toObject()["artist"].toString();
            int track_num = track.toObject()["metadata"].toObject()["track_number"].toInt();
            QString strLength = track.toObject()["metadata"].toObject()["length"].toString();
            std::string stdStrLength = strLength.toStdString();
            QString genre = track.toObject()["metadata"].toObject()["genre"].toString();
            int mins = 0;
            int secs = 0;
            int index = 0;
            while (strLength[index].isNumber())
            {
                mins = mins * 10 + atoi(stdStrLength.substr(index, 1).c_str());
                index++;
            }
            index++;
            while (strLength[index].isNumber())
            {
                secs = secs * 10 + atoi(stdStrLength.substr(index, 1).c_str());
                index++;
            }
            int length = mins * 60 + secs;

            playlist_item *tmpItem = new playlist_item(songName, hash, length, album, artist, track_num, genre);
            tmp->addSong(*tmpItem);
        }
        this->addPlaylist(*tmp);
    }
}

/*!
 * \brief playlist_handler::playSingleSong Creates a new playlist for a single song and plays a single song
 * \param name The name of the song to be played
 * \param hash The hash of the song's actual audio location
 * \param artist The artist of the piece
 * \param album The album, if any, that the piece was recorded on
 * \param length The length of the song in seconds
 * \param genre The genres that describe the song, seperated by commas
 * \param trackNumber The position of the song on its album, if any
 */
void playlist_handler::playSingleSong(QVariant name, QVariant hash, QVariant artist, QVariant album,
                                      int length, QVariant genre, int trackNumber)
{
    qDebug() << "playSingleSong(...) called";
    emit loadingSong();
    this->pause();
    this->dropPlaylist("Single Song");
    this->addPlaylist(QString("Single Song"));
    this->activePlaylist = this->playlists.size()-1;
    this->addSong(name, hash, artist, album, length, genre, trackNumber);
    emit requestSong(hash.toString());
    emit activePlaylistChanged("Single Song");
}
