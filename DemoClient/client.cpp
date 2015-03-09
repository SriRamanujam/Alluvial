#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //hardcoding these for now since i'm lazy
    QString SERVER_HOST = "192.168.2.7";
    qint64 SERVER_PORT = 8900;
    counter = 0;

    player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(printStatus(qint64)));


    initiateDownload();
}

Client::~Client()
{
    player->deleteLater();
}

void Client::printStatus(qint64 pos)
{
    qDebug() << "Current position is " << pos / 1000 / 60 << ":" << pos / 1000 % 60;
//    counter += 1;
//    if (counter > 14) {
//        player->setPosition(32000);
//        counter = 0;
//    }
}

void Client::requestFinished(QNetworkReply *reply)
{
    QByteArray file;
    QBuffer buf;
    if (reply->isFinished()) {
        buf = QBuffer(&reply->readAll(), this);
    }

    player->setMedia(QMediaContent(), &buf);
    player->setVolume(20);
    player->play();
}

void Client::initiateDownload()
{
    QNetworkRequest req = QNetworkRequest(QUrl("http://192.168.2.7:8900"));
    manager->get(req);
}

void Client::play()
{
//    player->setMedia(QUrl::fromLocalFile("/home/sri/Downloads/09-99-Problems.mp3"));
    player->setMedia(QMediaContent(QNetworkRequest(QUrl("http://192.168.2.7:8900"))));
    player->setVolume(20);

//    player->setPosition(34000);

    player->play();
}

