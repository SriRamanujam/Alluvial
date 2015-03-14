#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    player = new QMediaPlayer;

    qDebug() << "Starting data transfer";

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

    socket->open(QUrl("ws://192.168.2.7:8900"));

}

Client::~Client()
{
//    player->deleteLater();
}

void Client::debugPrintError(QAbstractSocket::SocketError error)
{
    qDebug() << "ERR: error is" << error;
}

///
/// \brief Placeholder method, meant to alert us if a text frame is
/// received since we don't currently assume that they are a factor
/// in our communication.
/// \param msg
///
void Client::onTextMessageReceived(QString msg)
{
    qDebug() << "TEXT FRAME RECEIVED:" << msg;
}

///
/// \brief Our initial binary message handler. Naively just assumes
/// the message contains music and tries to play it.
/// \param data
///
/// So for now this is a very naive method. Since for now we assume
/// that the only binary data arriving over this websocket shall be
/// music, we just write the data to a temp file and try to play it.
/// So far this has had a 100% success rate.
///
void Client::onBinaryMessageReceived(QByteArray data)
{
    QFile tmp_file("/run/user/1000/out.mp3");
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
    qDebug() << "Data write finished, now opening file";

    player->setMedia(QUrl::fromLocalFile("/run/user/1000/out.mp3"));
    qDebug() << player->errorString();
    player->setVolume(50);
    player->play();
    qDebug() << "playing file now :D :D :D";
}

///
/// \brief A debug method that generates some example data and sends it
/// to the server.
///
/// We use QStringList to hold a list of strings, which are then turned
/// into a QJsonArray, and fed into a QJsonObject. This QJsonObject
/// encapsulates the Qt binary representation of the JSON object
/// {
///     "properties": [
///         "me",
///         "mom",
///         "dad",
///         "sister"
///     ]
/// }
/// once all is said and done. We then convert the QJsonObject into
/// a QJsonDocument, which then allows us to render the binary representation
/// of this JSON object as serialized text using QJsonDocument::toJson(),
/// and sent as a text message over the websocket.
///
void Client::debugSendData()
{
    QStringList list;
    list << "me" << "mom" << "dad" << "sister";

    QJsonObject obj = QJsonObject();
    QJsonArray fam = QJsonArray::fromStringList(list);
    obj["properties"] = fam;
    QJsonDocument doc = QJsonDocument(obj);

    QString serialized = QString(doc.toJson());

    qDebug() << "Connected to " << socket->requestUrl();
    quint64 data_sent = socket->sendTextMessage(serialized);
    qDebug() << "Sent" << data_sent << "bytes of data";
}

void Client::debugCloseClient()
{
    qDebug() << "Server terminated connection, closing";
    player->deleteLater();
}

