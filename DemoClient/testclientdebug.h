#ifndef TESTCLIENTDEBUG_H
#define TESTCLIENTDEBUG_H

#include <QObject>
#include <QWebSocket>
#include "communicationhandler.h"

class TestClientDebug : public QObject
{
    Q_OBJECT
public:
    explicit TestClientDebug(QObject *parent = 0);
    ~TestClientDebug();

private:
    CommunicationHandler *handler;

signals:

public slots:
    void authReceivedCallback(bool auth);
    void searchReceivedCallback(QJsonObject obj);
};

#endif // TESTCLIENTDEBUG_H
