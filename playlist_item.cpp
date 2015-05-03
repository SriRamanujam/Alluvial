#include "playlist_item.h"
#include <QVariant>

/*!
 * \brief playlist_item::playlist_item The container for all of the song metadata.
 * This will contain all of the necessary display info for listing in the playlist bar,
 * as well as the hash of the song for look up purposes.
 *
 * This does not include metadata not used in the playlist bar as that will be returned
 * by the query of the database with the hash.
 */
playlist_item::playlist_item()
{
    this->hash = "";
    this->songName = "";
    this->songLength = 0;
    this->album = "";
    this->artist = "";
    this->track_number = 0;
    this->stringLength = "";
    this->genre = "";
}

/*!
 * \brief playlist_item::playlist_item Create a new playlist item with all of the metadata needed for playback
 * \param hash The hash of the song's audio location in the database
 * \param songName The song's name, as given to us from search results. Might be messy
 * \param length The length of the song in seconds. Will also generate a string for displaying.
 * \param album The album the song was a part of, if any.
 * \param artist The artists that played the song
 * \param track The track number, indicating its position on its album if any
 * \param genre The genres that describe the song seperated by commas
 */
playlist_item::playlist_item(QString hash, QString songName, int length,
        QString album, QString artist, int track, QString genre)
{
    this->hash = hash;
    this->songName = songName;
    this->songLength = length;
    this->album = album;
    this->artist = artist;
    this->track_number = track;
    QString mins = QVariant(length / 60).toString();
    int secs = length % 60;
    if ( secs < 10 )
    {
        this->stringLength = mins.append(":0").append(QVariant(secs).toString());
    }
    else
    {
        this->stringLength = mins.append(":").append(QVariant(secs).toString());
    }
    this->genre = genre;
}

/*!
 * \brief playlist_item::playlist_item (deprecated) A minimal functionality constructor.
 * \param hash The hash of the song's audio location in the database
 * \param songName The song's name, as given to us from search results. Might be messy
 * \param length The length of the song in seconds.
 */
playlist_item::playlist_item(QString hash, QString songName, int length)
{
    this->hash = hash;
    this->songName = songName;
    this->songLength = length;
    this->album = "";
    this->artist = "";
    this->track_number = 0;
    QString mins = QVariant(length / 60).toString();
    int secs = length % 60;
    if ( secs < 10 )
    {
        this->stringLength = mins.append(":0").append(QVariant(secs).toString());
    }
    else
    {
        this->stringLength = mins.append(":").append(QVariant(secs).toString());
    }
    this->genre = "";
}

playlist_item::~playlist_item()
{

}

/*!
 * \brief playlist_item::getHash Get the hash of the song for the database
 * \return The hash of the song as a QString
 */
QString playlist_item::getHash()
{
    return this->hash;
}

/*!
 * \brief playlist_item::getSongName Get the name of the song
 * \return The name of the song as a QString
 */
QString playlist_item::getSongName()
{
    return this->songName;
}

/*!
 * \brief playlist_item::getSongLength Get the length of the song in seconds
 * \return The length of the song in seconds
 */
int playlist_item::getSongLength()
{
    return this->songLength;
}

/*!
 * \brief playlist_item::setHash Set the hash of the song for the database
 * \param newHash The hash to assign
 */
void playlist_item::setHash(QString newHash)
{
    this->hash = newHash;
}

/*!
 * \brief playlist_item::setSongName Set the name of the song
 * \param newName The name to assign
 */
void playlist_item::setSongName(QString newName)
{
    this->songName = newName;
}

/*!
 * \brief playlist_item::setSongLength Set the length of the song in seconds
 * \param newLength The length of the song in seconds
 */
void playlist_item::setSongLength(int newLength)
{
    this->songLength = newLength;
}
