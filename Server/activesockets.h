#ifndef ACTIVESOCKETS_H
#define ACTIVESOCKETS_H

#include "clientconnection.h"
#include <QObject>

/*!
 * \brief The ActiveSockets class holds references to each client connection, along with
 * some basic getters and setters for easier access
 *
 * This class should only be instantiated once during the lifetime of the server
 * process, and should act as a singleton.
 */
class ActiveSockets : public QObject
{
    Q_OBJECT
public:
    explicit ActiveSockets(QObject *parent = 0);
    ~ActiveSockets();

    ClientConnection* getMostRecentlyUsedConn();
    const ClientConnection* getConnection(qint64 index);

    void addConnection(ClientConnection* conn);
    void deleteConnection(qint64 index);

signals:

public slots:
private:
    /*!
     * \brief The list of ClientConnections currently being managed
     * by the server.
     */
    QList<ClientConnection*> *connections;
};

#endif // ACTIVESOCKETS_H
