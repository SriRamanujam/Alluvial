/**
  @file
*/
#include "activesockets.h"

/*!
 * \brief This class holds references to each client connection, along with
 * some basic getters and setters for easier access. This class should only be
 * instantiated once per application.
 *
 * \param parent
 */
ActiveSockets::ActiveSockets(QObject *parent) : QObject(parent)
{
    connections = new QList<ClientConnection*>();
}

/*!
 * \brief ActiveSockets::~ActiveSockets
 */
ActiveSockets::~ActiveSockets()
{

}

/*!
 * \brief returns a reference to the most recently used connection.
 */
ClientConnection* ActiveSockets::getMostRecentlyUsedConn()
{
    // TODO: implement
}

/*!
 * \brief gets a specific connection, referenced by its index.
 * \param index
 */
const ClientConnection* ActiveSockets::getConnection(qint64 index)
{
    return connections->at(index);
}

/*!
 * \brief adds a connection to the list.
 * \param conn
 */
void ActiveSockets::addConnection(ClientConnection* conn)
{
    connections->append(conn);
}

/*!
 * \brief deletes the connection at the index.
 * \param index
 */
void ActiveSockets::deleteConnection(qint64 index)
{
    connections->removeAt(index);
}
