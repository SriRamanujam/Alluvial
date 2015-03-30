#include "client.h"

/*!
 * \brief Client::Client
 * \param parent
 */
Client::Client(QObject *parent) : QObject(parent)
{
    player = new QMediaPlayer;

    socket = new QWebSocket();
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(debugPrintError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()),
            this, SLOT(debugSendData()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(debugCloseClient()));
    connect(socket, SIGNAL(textMessageReceived(QString)),
            this, SLOT(onTextMessageReceived(QString)));
    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)),
            this, SLOT(onBinaryMessageReceived(QByteArray)));

    /*
     * This needs to be set dynamically, probably on startup, via a setting.
     * Jeff: things for you to do :)
     */
    socket->open(QUrl("ws://192.168.2.7:8900")); // TODO: be dynamic

}

Client::~Client()
{
//    player->deleteLater();
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
 * \brief Client::onTextMessageReceived
 * \param msg
 */
void Client::onTextMessageReceived(QString msg)
{
    qDebug() << "Text message received";

    QJsonDocument res = QJsonDocument::fromJson(msg.toUtf8());

    QString res_type = res.object()["response_type"].toString();

    /*
     * TODO: Jeff and Mitch, these two functions are just ones I made up.
     * You two have to sit down and decide how you want to handle the
     * presentation of the data contained within the JSON sent back by the
     * server. From there, you need to build out these functions accordingly.
     */
    if (res_type == "search")  {
        handleSearchResponse(res.object());
    } else if (res_type == "authentication") {
        handleAuthResponse(res.object());
    }
}

/*!
 * \brief Client::handleAuthResponse
 * \param obj
 */
void Client::handleAuthResponse(QJsonObject obj)
{
    return;
}

/*!
 * \brief Client::handleSearchResponse
 * \param obj
 */
void Client::handleSearchResponse(QJsonObject obj)
{

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

