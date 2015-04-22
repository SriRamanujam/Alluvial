#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include "settings_storage.h"

class JsonFormatter:public QObject
{
    Q_OBJECT
public:
    JsonFormatter(const QJsonObject);
    JsonFormatter();
     QByteArray getJSONSettings();
    ~JsonFormatter();

signals:
    void sendSettingsToServer(QByteArray settings);
    void jsonConversionCompleted();

public slots:
    //onValueChanged
    void onLoginRequest(QString username, QString password);

private:



};

#endif // JSONFORMATTER_H
