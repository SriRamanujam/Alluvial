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

    socket->open(QUrl("ws://192.168.2.7:8900"));

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
    qWarning() << "TEXT FRAME RECEIVED:" << msg;
}

/*!
 * \brief Client::onBinaryMessageReceived
 * \param data
 */
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

