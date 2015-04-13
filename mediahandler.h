#ifndef MEDIAHANDLER_H
#define MEDIAHANDLER_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class MediaHandler : public QObject
{
    Q_OBJECT
public:
    explicit MediaHandler(QObject *parent = 0);
    ~MediaHandler();
    QJsonObject search(QString query);

signals:

public slots:
private:
    unsigned int levenshtein_distance(QString s1, QString s2);
//    SpotifyHandler *spotify;
//    SoundCloudHandler *soundcloud;
//    DatabaseHandler *db;

private slots:
    void onSpotifySearchComplete(QJsonObject obj);
    void onSoundcloudSearchComplete(QJsonObject obj);
    void onDbSearchComplete(QJsonObject obj);
};

#endif // MEDIAHANDLER_H
