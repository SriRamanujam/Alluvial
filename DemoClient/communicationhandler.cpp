#include "communicationhandler.h"

/*!
 * \brief This module handles all client communication with the server. It abstracts
 * all the nitty-gritty details about sending data around, and presents a consistent
 * interface to the rest of the application.
 *
 * This module provides an interface that simply takes in the data structures used
 * by the client application and handles all serialization and transport. It also
 * provides signals to indicate when a new type of message is received.
 *
 * Note that this module retains ownership of the websocket. This means that this
 * module should be instantiated during application startup as a singleton. In
 * addition, no other module should have a connection to the socket.
 */
CommunicationHandler::CommunicationHandler(QString url, QObject *parent) : QObject(parent)
{
    connectToServer(url);
    qDebug() << "Connected to server, now awaiting orders, sire";
}


/*!
 * \brief Handles safe and secure destruction of all data structures used by
 * this module.
 */
CommunicationHandler::~CommunicationHandler()
{
    socket->deleteLater();
}

/*!
 * \brief This function handles serializing and sending an authentication request.
 * This function is called as a part of the communication handler's startup routine,
 * and every time a reconnection occurs.
 *
 * \param pass The password to be sent.
 */
void CommunicationHandler::sendAuthRequest(QString pass)
{

    QJsonObject obj
    {
        {"request_type", "authentication"}
    };

    QJsonObject inner
    {
        {"password", pass}
    };

    obj["request"] = inner;

    qDebug() << "Authentication request is:";
    qDebug() << QString(QJsonDocument(obj).toJson());


    socket->sendTextMessage(QJsonDocument(obj).toJson());
}

/*!
 * \brief This function is called every time a playlist needs to be synced to the server.
 * It serializes the playlist object passed in to JSON and sends it into the socket.
 *
 * \todo Once Jeff has playlist objects done, build out this method
 */
void CommunicationHandler::sendPlaylist()
{

}

/*!
 * \brief This method formats and sends search requests to the server.
 * Returns nothing, because error handling is done using a separate signal/slot
 * interface
 *
 * \param req The QString containing the search query.
 */
void CommunicationHandler::sendSearchRequest(QString req)
{
    QJsonObject inner
    {
        {"query", req}
    };

    QJsonObject obj
    {
        {"request_type", "search"}
    };

    obj["request"] = inner;

    qDebug() << "Search request is:";
    qDebug() << QString(QJsonDocument(obj).toJson());

    socket->sendTextMessage(QJsonDocument(obj).toJson());
}

/*!
 * \brief This function creates and sends a media request to the server.
 *
 * \param hash A QString containing the media hash of the song being requested.
 */
void CommunicationHandler::sendMediaRequest(QString hash)
{
    QJsonObject inner
    {
        {"hash", hash}
    };

    QJsonObject obj
    {
        {"request_type", "media"}
    };

    obj["request"] = inner;

    qDebug() << "Media request is:";
    qDebug() << QString(QJsonDocument(obj).toJson());

    socket->sendTextMessage(QJsonDocument(obj).toJson());
}

/*!
 * \brief This method handles any processing and logic needed to render playlist
 * objects usable by the greater application.
 *
 * \todo Implement this once Jeff gets playlist objects sorted out.
 *
 * \param obj The QJsonObject containing the playlist contents received from the server.
 */
void CommunicationHandler::handlePlaylistResponse(QJsonObject obj)
{
    if (obj["response"].toString() == "Error") {
        // an error has occurred, emit an error message
        emit playlistError(obj["error"].toString());
        return;
    }
}

/*!
 * \brief This method deserializes and parses authentication responses from the server,
 * and emits an error signal if something goes wrong. By design, this function remains
 * silent if the playlist request succeeded.
 *
 * \param obj The QJsonObject containing the response
 */
void CommunicationHandler::handleAuthResponse(QJsonObject obj)
{
    qDebug() << "We have received an auth response";
    if (obj["Response"].toString() ==  "Error") {
        // an error has occurred, emit an error message
        emit authError(obj["error"].toString());
        return;
    }

    if (obj["response"].toObject()["success"].toBool()) {
        qDebug() << "Auth request succeeded!";
        emit onAuthReceived(true);
        return;
    } else {
        qDebug() << "Auth request failed!";
        emit onAuthReceived(false);
        return;
    }
    emit authError("Unspecified authentication failure");
}


/*!
 * \brief This method parses search responses from the server, and emits the appropriate
 * signal based on the result.
 *
 * If the request has failed in any way, this method emits an error. Otherwise
 * it just emits a signal containing the search results.
 *
 * \todo Testing to figure out what could go wrong here.
 * \param obj
 */
void CommunicationHandler::handleSearchResponse(QJsonObject obj)
{
    if (obj["response"].toString() == "Error") {
        // an error has occurred, emit an error message
        emit searchError(obj["error"].toString());
        return;
    }
    emit onSearchReceived(obj);
}

/*!
 * \brief This method handles connection to the remote server. Given a fully 
 * qualified host name this method safely initiates a new connection to that server,
 * and negotiates the authentication handshake to ensure further data transmission.
 *
 * This method essentially everything needed for a socket connection to work,
 * namely hooking up signals and slots, error checking, etc. All of the core
 * connection flow is encapsulated in this method.
 * 
 * \param host The hostname (optionally including port) of the remote server.
 */
void CommunicationHandler::connectToServer(QString host)
{
    // we make sure that if an old socket instance exists, it is
    // completely and utterly destroyed before we move forward.
//    if (socket) {
//        socket->deleteLater();
//    }

    socket = new QWebSocket();

    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)),
            this, SLOT(handleBinaryMessage(QByteArray)));
    connect(socket, SIGNAL(textMessageReceived(QString)),
            this, SLOT(handleTextMessage(QString)));

    // the client should transparently try to reconnect to the server.
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(reconnectToServer()));

    // the client should automatically try to initiate login when we've connected
    connect(socket, &QWebSocket::connected,this, &CommunicationHandler::initiateAuthReq);

    // we now ensure the host is properly formatted, by creating the QUrl
    // and using it to validate the URL.
    QUrl hostUrl = QUrl::fromUserInput(host);
    if (!hostUrl.isValid()) {
        qWarning() << "Url invalid, what the hell Jeff validate your input!";
        return;
    }

    // ensures that we pass a websocket url no matter what
    hostUrl.setScheme("ws");

    socket->open(hostUrl);
}

void CommunicationHandler::initiateAuthReq()
{
    qDebug() << "Now sending initial auth request";
    sendAuthRequest("serverPass");
}

/*!
 * \brief This function is the slot called by the textMessageReceived() signal.
 * It deserializes the received document (which should be valid JSON) and handles
 * it appropriately according to its parsed content.
 *
 * \param doc the QString from the socket.
 *
 * \todo make this happen
 */
void CommunicationHandler::handleTextMessage(QString doc)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(doc.toUtf8());
    QJsonObject obj = jdoc.object();
    
    if (obj["response_type"].isString()) {
        QString type = obj["response_type"].toString();
        
        if (type == "search") {
            handleSearchResponse(obj);
;        } else if (type == "playlist") {
            handlePlaylistResponse(obj);
        } else if (type == "authentication") {
            handleAuthResponse(obj);
        }
    }
}

/*!
 * \brief This method is called when the socket loses connection. It marshals
 * all needed state and tries to re-establish the connection in a somewhat intelligent
 * manner.
 */
void CommunicationHandler::reconnectToServer()
{
    // we emit a signal just in case anything else want to know that we've
    // lost our connection
    emit connectionLost();

    // we then promptly try to reconnect.
    // TODO: implement some kind of delay or stop just in case this cycle
    // keeps looping forever
    qDebug() << "Disconnected from server :(";
    QHostAddress addr = socket->peerAddress();
    qDebug() << "We're going to try to reconnect now...";
    connectToServer(addr.toString());
}

/*!
 * \brief This method is a private slot that handles incoming binary data.
 * In Alluvial, binary data is exclusively going to be media data. So, then,
 * in essence, this slot is called every time the server sends a song.
 *
 * \param data The QByteArray representing the media data.
 */
void CommunicationHandler::handleBinaryMessage(QByteArray data)
{
    // topkek
    emit onMediaReceived(data);
}
