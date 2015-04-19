#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QObject>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>

#include "simplecrypt.h"

class SearchResult
{
    Q_OBJECT
public:
    explicit SearchResult(QString query);
    ~SearchResult();
    bool SEARCH_COMPLETE;

    QString query;

    QJsonObject getSearchResults();
signals:
    void searchProcessingComplete();

private:
    bool SPOTIFY_COMPLETE = false;
    bool SOUNDCLOUD_COMPLETE = false;
    bool DB_COMPLETE = false;
    SimpleCrypt *crypto;

    QJsonArray dbRes;
    QJsonArray scRes;
    QJsonArray spotifyRes;

    QJsonObject fullResult;

    void constructFullResult();
    void insertObjectsIntoResults(QJsonArray *arr);

public slots:
    void onSpotifySearchComplete(QJsonArray obj);
    void onSoundcloudSearchComplete(QJsonArray obj);
    void onDbSearchComplete(QJsonArray obj);
};

#endif // SEARCHRESULT_H
