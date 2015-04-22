#include "schandler.h"

SCHandler::SCHandler(QObject *parent)
{
    raw_results = QJsonArray();
    last_search = QJsonObject();
}


SCHandler::~SCHandler()
{

}


//category: artist, title, user,
void SCHandler::query(QString value, QString key){
    QJsonObject last_search {
        {key, value}
    };

    if(value == NULL)
        return;
    if(key == NULL)
        return;
    //check to prevent same query twice? nah don't be dumb

    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    QUrl url(SC_TRACKS_URL);
    QUrlQuery query;
    query.addQueryItem("client_id", SC_CLIENT_ID);

    query.addQueryItem("downloadable","true");
    query.addQueryItem(key, value);

    url.setQuery(query.query());
    qDebug() << url;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req(url);
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        QJsonParseError err;
        QJsonDocument jsondoc = QJsonDocument(QJsonDocument::fromJson(QString(reply->readAll()).toUtf8(), &err)); //raw string to qtstring to bytecode to jsondoc fucking shit
        raw_results = jsondoc.array(); //take the pile of responses and make them an array so you can fucking do something with the

        emit onSearchComplete(&raw_results);

    }
    else {
        //failure
        qDebug() << "Failed query: " <<reply->errorString();
        delete reply;
        return;
    }
}

QJsonObject SCHandler::format(QJsonValue initial){
    QJsonObject jobj = initial.toObject();
    QJsonObject media{
        {"hash",""},
        {"order",""},
    };

    QJsonObject meta{
        {"title", jobj["title"].toString()},
        {"album",""}, //nope because soundcloud
        {"artist", jobj["user"].toObject()["username"].toString()},
        {"track_number", 0},
        {"length", ""}, //get from fucking duration
        {"genre", jobj["genre"].toString()}
    };
    //add meta to media
    media["metadata"] = meta;

    return media;
}

QByteArray SCHandler::request_song(QString download_url, QString target){

    // create custom temporary event loop on stack
    QNetworkRequest request;
    request.setRawHeader("User-Agent", USER_AGENT);
    QString local_path;
    QEventLoop eventLoop;
    QString req_url = download_url;
    QUrl url(req_url);
    QUrlQuery query;

    query.addQueryItem("client_id", SC_CLIENT_ID);

    url.setQuery(query.query());

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req(url);

    //req.setRawHeader();
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called



    if (reply->error() == QNetworkReply::NoError) {
        //get temp download url as json

        QJsonDocument jsondoc = QJsonDocument(QJsonDocument::fromJson(QString(reply->readAll()).toUtf8()));
        QJsonObject jobj = jsondoc.object();
        QString download_url = jobj["location"].toString();

        //download
        if (reply->error() == QNetworkReply::NoError) {
            req_url = download_url;
            QUrl url(req_url);
            QNetworkRequest req(url);
            req.setRawHeader("User-Agent",USER_AGENT);
            QNetworkReply *reply = mgr.get(req);
            eventLoop.exec();

            QUrl aUrl(url);
            QFileInfo fileInfo=aUrl.path();
            qDebug() << fileInfo.fileName();
//            QFile file(target);
//            file.open(QIODevice::WriteOnly);
            return reply->readAll();
        }
        else{
            qDebug() << "Failure on download request" <<reply->errorString();
            delete reply;
            return QByteArray();
        }

    }
    else {
        //failure
        qDebug() << "Failure on initial request" <<reply->errorString();
        delete reply;
        return QByteArray();
    }
}
