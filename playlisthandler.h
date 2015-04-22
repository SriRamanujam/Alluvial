#ifndef PLAYLISTHANDLER_H
#define PLAYLISTHANDLER_H

#include <QObject>

class PlaylistHandler : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistHandler(QObject *parent = 0);
    ~PlaylistHandler();

signals:

public slots:
};

#endif // PLAYLISTHANDLER_H
