#include "clientconnection.h"

/*!
 * \brief This class encapsulates and represents a single client's connection.
 * It handles all logic and state regarding this client's communication to the
 * server, including such things as the socket itself and all dispatching logic.
 *
 * It essentially emits signals when new communication occurs, and gains control
 * of the event loop long enough to do its thing and then hand control back
 * to the main server process.
 * \param parent
 */
ClientConnection::ClientConnection(QWebSocket *sock, QObject *parent) : QObject(parent)
{
    socket = sock;
}

ClientConnection::~ClientConnection()
{

}

/*!
 * \brief Slot method called when a text message is received over the socket.
 * Deserializes the JSON code into a binary Qt object, processes it, retrieves
 * any results necessary, and sends that result through the socket. Essentially
 * this method serves as the main point of entry into each instance of this
 * class.
 *
 * Right now it just sends a hardcoded binary file as a response to every request
 * as a test proof-of-concept. In the future it will have its full suite of
 * functionality.
 */
void ClientConnection::onTextMessageReceived(QString doc)
{
    qDebug("Received request from client");
    qDebug() << doc;

    // I have no idea what this line does, but it gets us the sender's socket.
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        connect(client, SIGNAL(disconnected()),
                client, SLOT(deleteLater()));
    } else {
        qWarning() << "socket not retrieved, aborting early.";
        return;
    }

    QJsonDocument json = QJsonDocument::fromJson(doc.toUtf8());
    QJsonObject obj = json.object();

    qDebug() << obj["request_type"];

    if (obj["request_type"].isString()) {
        QString type = obj["request_type"].toString();

        if (type == "song") {
            // TODO: replace with actual logic
            QFile file("/home/sri/Downloads/sugar.mp3");
            file.open(QIODevice::ReadOnly);
            QByteArray data = file.readAll();
            qDebug() << "Read file in, now sending";
            client->sendBinaryMessage(data);
            return;
        } else if (type == "search") {
            QJsonDocument res = debugDemoSearch(obj); // TODO: change
            client->sendTextMessage(QString(res.toJson()));
            return;
        } else {
            QJsonDocument res = buildErrorMsg(MessageParseError::InvalidRequestType);
            client->sendTextMessage(QString(res.toJson()));
            return;
        }
    } else {
        QJsonDocument res = buildErrorMsg(MessageParseError::InvalidRequestType);
        client->sendTextMessage(QString(res.toJson()));
        return;
    }
}

QJsonDocument ClientConnection::debugDemoSearch(QJsonObject obj)
{

}

