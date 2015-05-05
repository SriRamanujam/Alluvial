#ifndef SONGHANDLER_H
#define SONGHANDLER_H
#include <QByteArray>
#include "databasehandler.h"

/*!
 * \brief The songHandler class simply gets songs for the client based
 * on song requests.
 */
class songHandler
{
public:
    songHandler();
    ~songHandler();
    QByteArray getSong(int PKID);
};

#endif // SONGHANDLER_H
