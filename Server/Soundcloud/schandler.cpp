#include "schandler.h"

SCHandler::SCHandler(QObject *parent)
{
    raw_results = QJsonArray();
}


SCHandler::~SCHandler()
{

}


//category: artist, title, user,
int SCHandler::query(QString key, QString value){

    if(value == NULL)
        return -1;
    if(key == NULL)
        return -1;
    //check to prevent same query twice? nah don't be dumb

    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    QUrl url(SC_TRACKS_URL);
    QUrlQuery query;
    query.addQueryItem("downloadable", "true");
    query.addQueryItem(key, value);

    query.addQueryItem("client_id", SC_CLIENT_ID);



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
//        qDebug() << raw_results;

    return raw_results.size();

    }
    else {
        //failure
        qDebug() << "Failed query: " <<reply->errorString();
        delete reply;
        return -1;
    }
}

QJsonValue SCHandler::format(QJsonValue initial){
    QJsonObject jobj = initial.toObject();
    QString length;
    if (jobj["duration"].toInt() > 0){
        QString minutes;
        QString seconds;
        int duration = jobj["duration"].toInt()/1000; //ain't nobody got time for accuracy
        minutes = QString::number(duration/60);
        seconds = QString::number(duration%60);
        if(duration/60 < 10)
            minutes = QString("0"+minutes);
        if(duration%60 < 10)
            seconds = QString("0"+seconds);

        length = QString(minutes+":"+seconds);
    }

    QJsonObject media{
        {"hash", jobj["download_url"].toString()},
        {"order",""},
    };

    QJsonObject meta{
        {"title", jobj["title"].toString()},
        {"album",""}, //nope because soundcloud
        {"artist", jobj["user"].toObject()["username"].toString()},
        {"track_number", 0},
        {"length", length}, //get from fucking duration
        {"genre", jobj["genre"].toString()}
    };
    //add meta to media
    media["metadata"] = meta;

    return QJsonValue(media);
}

QJsonArray SCHandler::search(QString value, QString key){
    QJsonArray results = QJsonArray();
    QJsonObject jobj;
    QString result;
    int num_queried = query(key, value);
    for(int i=0; i<num_queried; i++){
        jobj = raw_results[i].toObject();
        result = jobj["download_url"].toString();
//        qDebug() << result.compare(QString(""));
        if(result.compare(QString("")) != 0)
            results.append(format(raw_results[i]));
    }

    emit onSearchComplete(&results);
    return results;
}

QJsonArray SCHandler::search(int count, QString value, QString key){
    QJsonArray results = QJsonArray();
    int num_queried = query(key, value);
    for(int i=0; i<num_queried; i++){
        jobj = raw_results[i].toObject();
        result = jobj["download_url"].toString();
        qDebug() << result.compare(QString(""));
        if(result.compare(QString("")) != 0)
            results.append(format(raw_results[i]));
    }
    if(count > results.size())
        count = results.size();
    for(int i=0; i<count; i++){
        results.append(format(raw_results[i]));
    }
    emit onSearchComplete(&results);
    return results;
}

QByteArray SCHandler::request_song(QString download_url){
    // create custom temporary event loop on stack
    QNetworkRequest request;
    request.setRawHeader("User-Agent", USER_AGENT);
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

            return reply->readAll();

        }
        else{
            qDebug() << "Failure on download request" <<reply->errorString();
            delete reply;
            return NULL;
        }

    }
    else {
        //failure
        qDebug() << "Failure on initial request" <<reply->errorString();
        delete reply;
        return NULL;
    }
}


