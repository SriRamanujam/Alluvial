/*!
  @file
*/
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
 * \param sock The socket of the client connection this instance of the object will
 * be handling.
 */
ClientConnection::ClientConnection(QWebSocket *sock, QObject *parent) : QObject(parent)
{
    socket = sock;
}

/*!
 * \brief Handles safe and secure cleanup of any instance variables or references
 * held by this object.
 */
ClientConnection::~ClientConnection()
{
    socket->deleteLater();
}

/*!
 * \brief Slot method called when a text message is received over the socket.
 * Deserializes the JSON code into a binary Qt object, processes it, retrieves
 * any results necessary, and sends that result through the socket. Essentially
 * this method serves as the main point of entry into each instance of this
 * class.
 *
 * \param doc QString representing the text message received from the client.
 */
void ClientConnection::onTextMessageReceived(QString doc)
{
    qDebug("Received request from client");
    qDebug() << doc;

    // I have no idea what this line does, but it gets us the sender's socket.
//    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    QJsonDocument json = QJsonDocument::fromJson(doc.toUtf8());
    QJsonObject obj = json.object();

    qDebug() << obj["request_type"];

    if (obj["request_type"].isString()) {
        QString type = obj["request_type"].toString();

        if (type == "media") {
            _handleMediaReq(obj);
            return;
        } else if (type == "search") {
            _handleSearchReq(obj);
            return;
        } else if (type == "settings") {
            _handleSettingsReq(obj);
        } else {
            QJsonDocument res = buildErrorMsg(MessageParseError::InvalidRequestType);
            socket->sendTextMessage(QString(res.toJson()));
            return;
        }
    } else {
        QJsonDocument res = buildErrorMsg(MessageParseError::InvalidRequestType);
        socket->sendTextMessage(QString(res.toJson()));
        return;
    }
}

/*!
 * \brief ClientConnection::_handleSettingsReq
 * \param req
 */
void ClientConnection::_handleSettingsReq(QJsonObject req)
{

}

/*!
 * \brief This method handles authentication. It extracts the password, checks it against
 * the server's stored password, and sends an appropriate response. Because there is absolutely
 * no crypto involved (yet) in this transaction, this ends up being a huge wrapper around
 * a string comparison. This is roadmapped to change in the future.
 *
 * \param req request object received from the client.
 * \return
 */
void ClientConnection::_handleAuthenticationReq(QJsonObject req)
{
    qDebug() << "We are now handling an authentication request";

    QJsonObject res;
    QJsonObject result;
    res["response_type"] == "authentication";

    QString pass = req["request"].toObject()["password"].toString();

    // placeholder line, but you get the gist.
    if (pass == serverPass) {
        result["success"] = true;
    } else {
        result["success"] = false;
    }

    res["response"] = result;

    socket->sendTextMessage(QJsonDocument(res).toJson());
}

/*!
 * \brief This method handles search requests. It extracts the query from the request,
 * hands the query string to the search handler and sets up a callback for when the
 * search completes.
 *
 * \param req QJsonObject representing the request from the client.
 * \return
 */
void ClientConnection::_handleSearchReq(QJsonObject req)
{
    qDebug() << "We are now handling a search request";

    QJsonObject res;

    // TODO: error handling here, in case some part of this doesn't work.
    QString query = req["request"].toObject()["query"].toString();

    // placeholder line, but API should be the same.
    QJsonObject response = mediaHandler->search(query);


    res["response_type"] = "search";
    res["query"] = query;
    res["results"] = response;

    socket->sendTextMessage(QJsonDocument(res).toJson());
}

/*!
 * \brief This function provides a brief method to retrieve a media file from
 * the mediaHandler module. It uses the media object's unique hash to retrieve
 * the most optimal version of the source file.
 *
 * \param req QJsonObject representing the full request from the client.
 */
void ClientConnection::_handleMediaReq(QJsonObject req)
{
    qDebug() << "We are now handling a media request";

    QString hash = req["request"].toObject()["hash"].toString();

    // placeholder code, but you get the idea.
    QByteArray media = mediaHandler->getMediaFromHash(hash);

    socket->sendBinaryMessage(media);
}

/*!
 * \brief This method builds error responses from instances of MessageParseError.
 * Typically this method is called when program flow enters into an error state
 * and the server needs to communicate this to the client.
 *
 * \param err MessageParseError that was encountered.
 * \return A QJsonDocument representing the JSON-formatted error response.
 */
QJsonDocument ClientConnection::buildErrorMsg(MessageParseError err)
{
    QJsonObject errorObj;
    QString type = mostRecentObj["request_type"].toString(); // racy
    errorObj["response_type"] = type;
    errorObj["response"] = "Error";

    // Build error message
    switch(err) {
    case MessageParseError::UnknownError:
        errorObj["error"] = "Unknown Error";
        break;
    case MessageParseError::InvalidRequestType:
        errorObj["error"] = "Invalid request type";
        break;
    case MessageParseError::ParseError:
        errorObj["error"] = "Invalid JSON";
        break;
    }

    return QJsonDocument(errorObj);
}

