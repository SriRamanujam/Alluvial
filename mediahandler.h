#ifndef MEDIAHANDLER_H
#define MEDIAHANDLER_H
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QStringList>
#include <QMap>

#include "searchresult.h"
#include "simplecrypt.h"

class MediaHandler : public QObject
{
    Q_OBJECT
public:
    explicit MediaHandler(QObject *parent = 0);
    ~MediaHandler();

    QJsonObject search(QString query);
    QByteArray getMediaFromHash(QString hash);
    static SimpleCrypt *crypto;

signals:
    void searchResultComplete(QJsonObject res);


public slots:
private:
    SpotifyHandler *spotify;
    SoundCloudHandler *soundcloud;
    DatabaseHandler *db;
    QQueue<SearchResult*> *searchQueue;
    QMap<QString, SearchResult*> *completedSearches;
    
    unsigned int levenshtein_distance(QString s1, QString s2);

private slots:
    void processQueue();

};

#endif // MEDIAHANDLER_H
