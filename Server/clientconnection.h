#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QJsonObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

#include "mediahandler.h"

/*!
 * \brief The MessageParseError enum represents the various types of errors that
 * can happen when de-serializing message data.
 */
enum class MessageParseError {
    UnknownError = 0,        /*!< Unknown error type. */
    InvalidRequestType = 1, /*!< Type does not match the data contained within the object. */
    ParseError = 2          /*!< JSON parsing error */
};

/*!
 * \brief The ClientConnection class encapsulates and represents a single client's
 * connection.
 *
 * It handles all logic and state regarding this client's communication to the
 * server, including such things as the socket itself and all dispatching logic.
 *
 * It essentially emits signals when new communication occurs, and gains control
 * of the event loop long enough to do its thing and then hand control back
 * to the main server process.
 */
class ClientConnection : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnection(QWebSocket *sock, MediaHandler *mediaHandler, QObject *parent = 0);
    ~ClientConnection();

signals:
public slots:
    void onTextMessageReceived(QString doc);

private slots:
    void returnSearchResponse(QJsonObject response);

private:
    /*!
     * \brief The socket to the client.
     */
    QWebSocket *socket;
    /*!
     * \brief A pointer to the mediaHandler instantiated by the server.
     */
    MediaHandler *mediaHandler;
    /*!
     * \brief A pointer to the most recent json object, just in case.
     */
    QJsonObject mostRecentObj;

    //parsing methods
    void _handleAuthenticationReq(QJsonObject req);
    void _handleSearchReq(QJsonObject req);
    void _handleMediaReq(QJsonObject req);

    //oooo nobody should see this
    void _handleSettingsReq(QJsonObject req);

    //build error message
    QJsonDocument buildErrorMsg(MessageParseError err);
};

#endif // CLIENTCONNECTION_H
