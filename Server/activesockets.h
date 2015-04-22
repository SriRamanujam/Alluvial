#ifndef ACTIVESOCKETS_H
#define ACTIVESOCKETS_H

#include "clientconnection.h"
#include <QObject>

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
    QList<ClientConnection*> *connections;
};

#endif // ACTIVESOCKETS_H
