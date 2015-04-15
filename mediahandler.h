#ifndef MEDIAHANDLER_H
#define MEDIAHANDLER_H
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QMap>

#include "searchresult.h"

class MediaHandler : public QObject
{
    Q_OBJECT
public:
    explicit MediaHandler(QObject *parent = 0);
    ~MediaHandler();
    void search(QString query);

    unsigned int levenshtein_distance(QString s1, QString s2);

signals:
    void searchResultComplete(QJsonObject res);


public slots:
private:
    SpotifyHandler *spotify;
    SoundCloudHandler *soundcloud;
    DatabaseHandler *db;
    QQueue<SearchResult*> *searchQueue;
    QMap<QString, SearchResult*> *completedSearches;

    void processQueue();

private slots:

};

#endif // MEDIAHANDLER_H
