#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

enum RequestType {
    INVALID=-1,
    GET=0,
    POST=1
};


class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QTcpSocket *socket = 0, QObject *parent = 0);
    ~HttpRequest();

    RequestType getRequestType();
    QString getRoute();
    QString getBodyAsQString();
    QByteArray* getBodyAsRaw();
    QTcpSocket* getSocket();
signals:

public slots:

private slots:
    void readIncomingRequest();
private:

    QTcpSocket *s;
    QMap<QString, QString> headers;
    RequestType type;
    QString route;
    QByteArray body;
};

#endif // HTTPREQUEST_H
