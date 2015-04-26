#include "mediaplayer.h"
#include <QByteArray>
#include <QFile>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QDir>
#include <QQmlComponent>
#include <QQmlApplicationEngine>

/*!
 * \brief mediaPlayer::mediaPlayer mediaPlayer will handle all of the music playback.
 * This will control volume, song playback, and song positioning.
 *
 * Linked directly to UI button clicks through the use of Public Slots.
 * Will also change properties of certain UI elements based off of the current song playing.
 *
 * When given a new ByteStream, the mediaPlayer will start it as a new song
 * but keep all settings that were actively being used.
 */
mediaPlayer::mediaPlayer()
{
    player = new QMediaPlayer();
    this->player->setNotifyInterval(1000);
    QObject::connect(this->player,SIGNAL(positionChanged(qint64)),
        this, SLOT(childPositionChanged(qint64)));
}

mediaPlayer::~mediaPlayer()
{

}

/*!
 * \brief mediaPlayer::play Continue the song to play
 */
void mediaPlayer::play ()
{
    player->play();
}

/*!
 * \brief mediaPlayer::play Play a brand new song
 * \param data The data of the song to play
 */
void mediaPlayer::play (QByteArray data)
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
    qDebug() << "Media set. Song duration:" << player->duration();
    player->setVolume(50);
    player->play();
}

/*!
 * \brief mediaPlayer::pause Pause the current song
 */
void mediaPlayer::pause ()
{
    player->pause();
}

/*!
 * \brief mediaPlayer::playOrPause Switch the playback state
 */
void mediaPlayer::playOrPause()
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
 * \brief mediaPlayer::skipTo Jump to a specific position in the song
 * \param position The position to jump to in milliseconds
 */
void mediaPlayer::skipTo(int position)
{
    // position in milliseconds
    player->setPosition(position);
}

/*!
 * \brief mediaPlayer::setVolume Change the playback volume
 * \param vol The value to change to on a scale of 1 - 100
 */
void mediaPlayer::setVolume(int vol)
{
    player->setVolume(vol);
}

void mediaPlayer::startFastForward()
{
    player->setVolume(player->volume() / 3);
    player->setPlaybackRate(6);
    qDebug() << "Playback rate = 6";
    qDebug() << this->player->position();
}

void mediaPlayer::startRewind()
{
    player->setVolume(player->volume() / 3);
    player->pause();
    qDebug() << "Playback rate = -6";
    qDebug() << this->player->position();
}

void mediaPlayer::resetPlaybackRate()
{

    player->setVolume(player->volume() * 3);
    player->play();
    player->setPlaybackRate(1);
    qDebug() << "Playback rate reset";
    qDebug() << this->player->position();
}

void mediaPlayer::childPositionChanged(qint64 position)
{
    qDebug() << "childPositionChanged called:" << position;
    emit positionChanged(position);
}
