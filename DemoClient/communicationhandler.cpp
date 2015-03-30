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
CommunicationHandler::CommunicationHandler(QObject *parent) : QObject(parent)
{

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
    QJsonObject obj = new QJsonObject();
    obj["request_type"] = "authentication";
    obj["request"] = new QJsonObject();
    obj["request"].toObject()["password"] = pass;

    socket->sendTextMessage(QString(QJsonDocument(obj).toJson()));
}

/*!
 * \brief This function is called every time a playlist needs to be synced to the server.
 * It serializes the playlist object passed in to JSON and sends it into the socket.
 *
 * TODO: Once Jeff has playlist objects done, build out this method
 */
void CommunicationHandler::sendPlaylist()
{

}

/*!
 * \brief This method validates, formats, and sends search requests to the server.
 * Returns nothing, because error handling is done using a separate signal/slot
 * interface
 *
 * \param req The QString containing the search query.
 */
void CommunicationHandler::sendSearchRequest(QString req)
{
    QJsonObject obj = new QJsonObject();
    obj["request_type"] = "search";
    obj["request"] = new QJsonObject();
    obj["request"].toObject()["query"] = req;

    socket->sendTextMessage(QString(QJsonDocument(obj).toJson()));
}

/*!
 * \brief This function creates and sends a media request to the server.
 *
 * \param hash A QString containing the media hash of the song being requested.
 */
void CommunicationHandler::sendMediaRequest(QString hash)
{
    QJsonObject obj = new QJsonObject();
    obj["request_type"] = "media";
    obj["request"] = new QJsonObject();
    obj["request"].toObject()["hash"] = hash;

    socket->sendTextMessage(QString(QJsonDocument(obj).toJson()));
}

/*!
 * \brief This method handles connection to the remote server. Given a fully 
 * qualified host name this method safely initiates a new connection to that server.
 *
 * This method essentially everything needed for a socket connection to work,
 * namely hooking up signals and slots, error checking, etc. All of the core
 * connection flow is encapsulated in this method.
 * 
 * \param host The hostname (optionally including port) of the remote server.
 */
void CommunicationHandler::connectToServer(QString host)
{
    // we make sure that if an old socket instance exists, that it is
    // completely and utterly destroyed before we move forward.
    if (socket) {
        socket->deleteLater();
    }

    socket = new QWebSocket();

    connect(socket, SIGNAL(binaryMessageReceived(QByteArray)),
            this, SLOT(handleBinaryMessage(QByteArray)));
    connect(socket, SIGNAL(textMessageReceived(QString)),
            this, SLOT(handleTextMessage(QString)));

    // the client should transparently try to reconnect to the server.
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(reconnectToServer()));

    // we now ensure the host is properly formatted, by creating the QUrl
    // and using it to validate the URL.
    QUrl hostUrl = QUrl::fromUserInput(host);
    if (!hostUrl.isValid()) {
        qWarning() << "Url invalid, what the hell Jeff validate your input!";
    }
    hostUrl.setScheme("ws");

    socket->connect(hostUrl);
}

/*!
 * \brief This function is the slot called by the textMessageReceived() signal.
 * It deserializes the received document (which should be valid JSON) and handles
 * it appropriately according to its parsed content.
 *
 * \param doc the QString from the socket.
 *
 * TODO: make this happen
 */
void CommunicationHandler::handleTextMessage(QString doc)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(doc.toUtf8());
    QJsonObject obj = jdoc.object();
    
    if (obj["response_type"].isString()) {
        QString type = obj["response_type"].toString();
        
        if (type == "search") {
            emit onSearchReceived(jdoc);
        } else if (type == "")
    }
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
