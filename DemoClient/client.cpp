#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    commsHandler = new CommunicationHandler("127.0.0.1");
}

Client::~Client()
{
    commsHandler->deleteLater();
}

/*!
 * \brief Client::debugPrintError
 * \param error
 */
void Client::debugPrintError(QAbstractSocket::SocketError error)
{
    qDebug() << "ERR: error is" << error;
}


/*!
 * \brief This function is called when the websocket receives a binary message.
 * For the purposes of the Alluvial client, the only thing that will be sent as
 * a binary message will be a song, so this function essentially serves as the
 * entry point for media playback once the file has been received from the server.
 *
 * Right now, it writes the song to a temp file, then sets up the QMediaPlayer
 * instance to play from that file. Fairly simple.
 *
 * \todo Needs to be able to dynamically choose a good temp location. Not every
 * computer will have the /run filesystem set up. How do other applications
 * do this?
 *
 * \param data The media file in the form of a QByteArray
 */
void Client::onBinaryMessageReceived(QByteArray data)
{
    QFile tmp_file("/run/user/1000/out.mp3"); // TODO: don't hardcode this
    if(!tmp_file.open(QIODevice::WriteOnly)) {
        qDebug() << "opening location didn't work";
        return;
    }

    qDebug() << "Now starting to write data of size" << data.size();

    if(tmp_file.write(data) < 0) {
        qDebug() << "file write failed" << tmp_file.errorString();
        return;
    }
    tmp_file.close();

    // now we play the file
    qDebug() << "Data write finished, now opening file";

    player->setMedia(QUrl::fromLocalFile("/run/user/1000/out.mp3"));
    qDebug() << player->errorString();
    player->setVolume(50);
    player->play();
    qDebug() << "playing file now :D :D :D";
}

/*!
 * \brief Client::debugSendData
 */
void Client::debugSendData()
{
    QStringList list;
    list << "me" << "mom" << "dad" << "sister";

    QJsonObject obj = QJsonObject();
    QJsonObject request = QJsonObject();
    request["type"] = "song";
    request["title"] = "sugar.mp3";
    QJsonArray fam = QJsonArray::fromStringList(list);
    obj["properties"] = fam;

    QJsonDocument doc = QJsonDocument(obj);

    QString serialized = QString(doc.toJson());

    qDebug() << "Connected to " << socket->requestUrl();
    quint64 data_sent = socket->sendTextMessage(serialized);
    qDebug() << "Sent" << data_sent << "bytes of data";
}

/*!
 * \brief Client::debugCloseClient
 */
void Client::debugCloseClient()
{
    qDebug() << "Server terminated connection, closing";
    player->deleteLater();
}

