#ifndef PLAYLIST_ITEM_H
#define PLAYLIST_ITEM_H
#include <QString>

class playlist_item
{
public:
    playlist_item();
    playlist_item(QString, QString, int);
    playlist_item(QString, QString, int, QString, QString, int, QString);
    ~playlist_item();

    QString getHash();
    QString getSongName();
    int getSongLength();

    void setHash(QString);
    void setSongName(QString);
    void setSongLength(int);

    //! The album the song was on, if any
    QString album;
    //! The artists who were featured on the track.
    QString artist;
    //! The position of the song in the album it was on, if it was on any
    int track_number;
    //! The length of the song in a string for displaying purposes
    QString stringLength;
    //! The genres that describe the song, seperated by commas
    QString genre;

private:
    //! A songs unique identifier
    /*! The hash that references where the song reference is saved in the server's database. */
    QString hash;
    //! The title of the song
    /*! The title of the song that will be used when displaying a playlists' tracks in the PlaylistPanel */
    QString songName; //< The title of the song for displaying purposes
    //! The length of the song
    /*! The length of a song saved in seconds, that may or may not be used when displaying a playlists' tracks */
    int songLength; //< The song length in int form. 1:20 song would be 80

};

#endif // PLAYLIST_ITEM_H
