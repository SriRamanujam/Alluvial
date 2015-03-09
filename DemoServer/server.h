#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QFile>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:

public slots:

private slots:
    void handleResponse();
//    void debugPrintSocketResults();
private:
    QTcpServer *server;
    void initServer();
    void handleGET(QTcpSocket* socket);
    void handlePOST(QTcpSocket* socket);
};

#endif // SERVER_H
