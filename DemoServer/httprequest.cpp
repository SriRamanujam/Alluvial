#include "httprequest.h"

HttpRequest::HttpRequest(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    this->s = socket;
    type = INVALID;

    // um so this is stupid. when the socket gets disconnected, we need to
    // make some very hard decisions about whether the Request object needs
    // to outlive it.
    // TODO: fix
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    connect(socket, SIGNAL(readyRead()), this, SLOT(readIncomingRequest()));
}


void HttpRequest::readIncomingRequest()
{
    int i;
    int j;
    QByteArray raw_header = s->readLine();
    qDebug() << raw_header;
    QString header_line_str;
    QStringList request_split;

    // we start by parsing the first incoming line for the request type
    // and route. If we don't find anything, we immediately drop the connection.
    // We also take this time to set the internal type of the request for later
    // use.
    request_split = QString(raw_header).trimmed().split(' ');
    if (request_split.at(0) == "GET") {
        qDebug() << "GET request received";
        type = GET;
    } else if (request_split.at(0) == "POST") {
        qDebug() << "POST request received";
        type = POST;
    } else {
        qWarning() << "Unsupported request type";
        s->disconnectFromHost();
    }

    route = request_split.at(1);
    qDebug() << "route is" + route;

    // Now we iterate through the rest of the socket buffer, reading it in
    //line-by-line until we're done parsing the headers.
    while (s->bytesAvailable() > 0) {
        header_line_str = QString(s->readLine()).trimmed();
        qDebug() << "Raw header line is " << header_line_str;
        if (header_line_str.trimmed().isEmpty()) {
            qDebug() << "Done parsing headers";
            break;
        }
        i = header_line_str.indexOf(":");
        headers[header_line_str.left(i)] = header_line_str.right(
                    header_line_str.size() - i - 1).trimmed();
    }

    qDebug() << "headers are\n";
    for (j = 0; j < headers.keys().size(); j++) {
        qDebug() << headers.keys().at(j) << ":" << headers[headers.keys().at(j)];
    }

    // the rest of the socket's buffer is assumed to be the body of the request.
    // GET requests are by definition going to be empty, but POST requests
    // will have data in them.
    body = s->readAll();

    qDebug() << "body is\n" << body;
}

RequestType HttpRequest::getRequestType()
{
    return type;
}

QString HttpRequest::getRoute()
{
    return route;
}

QString HttpRequest::getBodyAsQString()
{
    return QString(body);
}

QByteArray* HttpRequest::getBodyAsRaw()
{
    return &body;
}

QTcpSocket* HttpRequest::getSocket()
{
    return s;
}

HttpRequest::~HttpRequest()
{

}

