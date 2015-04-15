#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QObject>
#include <QCryptographicHash>

class SearchResult
{
    Q_OBJECT
public:
    explicit SearchResult(QString query);
    ~SearchResult();
    bool SEARCH_COMPLETE;

    QJsonObject getSearchResultObject();
signals:

private:
    QCryptographicHash *hasher;
    bool SPOTIFY_COMPLETE = false;
    bool SOUNDCLOUD_COMPLETE = false;
    bool DB_COMPLETE = false;

    QJsonObject dbRes;
    QJsonObject scRes;
    QJsonObject spotifyRes;

    QJsonObject fullResult;

    void constructFullResult();
j
public slots:
    void onSpotifySearchComplete(QJsonObject obj);
    void onSoundcloudSearchComplete(QJsonObject obj);
    void onDbSearchComplete(QJsonObject obj);
};

#endif // SEARCHRESULT_H
