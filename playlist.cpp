#include "playlist.h"
#include <QObject>
#include <QDebug>

/*!
 * \brief playlist::playlist A playlist item to be used as a wrapper for all of the song metadata.
 * Song metadata will be saved in a playlist_item that will be kept in a list.
 * Each playlist will have its own title that will be used for display purposes.
 */
playlist::playlist()
{
    this->title = "Untitled Playlist";
    this->songs = QVector<playlist_item>();
}

playlist::playlist(QString title)
{
    this->title = title;
    this->songs = QVector<playlist_item>();
}
playlist::~playlist()
{

}

/*!
 * \brief A function to relocate a song in the playlist
 * \param songToMove The song that is to be relocated
 * \param newPosition Where we want the song to end up in the playlist
*/
void playlist::moveSong(playlist_item songToMove, int newPosition)
{
    QString deleteSongHash = songToMove.getHash();
    for ( int index = 0; index < this->songs.size() ; index++ )
    {
        if ( this->songs[index].getHash() == deleteSongHash )
        {
            this->songs.erase(this->songs.begin() + index);
            break;
        }
    }
    this->songs.insert(this->songs.begin() + newPosition, songToMove);
}

/*!
 * \brief playlist::addSong Add a song to the start of the playlist
 * \param newSong The song to be added
 */
void playlist::addSong(playlist_item newSong)
{
    this->songs.insert(this->songs.begin(), newSong);
}

/*!
 * \brief playlist::removeSong Used to remove a song from the playlist
 * \param deleteSong The song to be deleted from the playlist
 */
void playlist::removeSong(playlist_item deleteSong)
{
    QString deleteSongHash = deleteSong.getHash();
    for ( int index = 0; index < this->songs.size() ; index++ )
    {
        if ( this->songs[index].getHash() == deleteSongHash )
        {
            this->songs.remove(index);
            break;
        }
    }
}

void playlist::removeSong(int songIndex)
{
    this->songs.remove(songIndex);
}

/*!
 * \brief playlist::getSongHash Get the hash of a song by its index
 * \param songIndex The index of the song to be gotten
 */
QString playlist::getSongHash(int songIndex)
{
    return this->songs[songIndex].getHash();
}

/*!
 * \brief playlist::getPlaylistTitle Get the title of the playlist
 * \return QString containing playlist title
 */
QString playlist::getPlaylistTitle()
{
    return this->title;
}

/*!
 * \brief playlist::name An alias of getPlaylistTitle
 * \return QString containing playlist title
 */
QString playlist::name()
{
    return this->title;
}

/*!
 * \brief playlist::getSongs Get the vector containing all songs
 * \return Vector containing the songs
 */
QVector<playlist_item> playlist::getSongs()
{
    return this->songs;
}

/*!
 * \brief playlist::getSongs Get the playlist item of a specified song
 * \return Playlist item of the chosen song
 */
playlist_item playlist::getSong(int index)
{
    return this->songs[index];
}

void playlist::setTime()
{
    QTime time;
    this->last_save = time.currentTime();
}

QTime playlist::getTime()
{
    return this->last_save;
}
