#include "client.h"



Client::Client(QObject *parent) : QObject(parent)
{
    //hardcoding these for now since i'm lazy
    QString SERVER_HOST = "192.168.2.7";
    qint64 SERVER_PORT = 8900;

    // wtf this works perfectly
    player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile("/home/sri/Downloads/09-99-Problems.mp3"));
    player->setVolume(20);

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(printStatus(qint64)));

    player->play();


}

Client::~Client()
{
    player->deleteLater();
}

void Client::printStatus(qint64 pos)
{
    qDebug() << "Current position is " << pos / 1000 / 60 << ":" << pos / 1000 % 60;
}

