#ifndef SONGHANDLER_H
#define SONGHANDLER_H
#include <QByteArray>
#include <databasehandler.h>

class songHandler
{
public:
    songHandler();
    ~songHandler();
    QByteArray getSong(int PKID);
};

#endif // SONGHANDLER_H
