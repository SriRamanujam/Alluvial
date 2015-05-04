#include "jsonformatter.h"
#include <stdio.h>
#include <QUrl>
#include <QJsonDocument>
#include <thread>
#include <QByteArray>
#include <QSettings>
#include "settings_storage.h"
#include <QDebug>


JsonFormatter::JsonFormatter(const QJsonObject)
{

}

JsonFormatter::JsonFormatter(){

}

JsonFormatter::~JsonFormatter()
{

}
/*
 *function to convert the INI settings file created by QSettings into
 * a properly formatted JSON document.
 *returns a QByteArray of the json document
 * */
/*!
 * \brief JsonFormatter::getJSONSettings  This function will convert the INI settings file created by QSettings into
 * a properly formatted JSON document and then converts it to binary.
 * \return Returns a QByteArray containing the JSON formatted version of the settings file.
 */
QByteArray JsonFormatter::getJSONSettings()
{
    Settings_storing *settings = new Settings_storing();
    QStringList keyListAll = settings->getAllKeys();
    QJsonObject settingsJson;

    for(int i=0; i< keyListAll.length();i++){

        QString key = keyListAll.at(i);
        QVariant value = settings->value(key, "empty");
        qDebug() << "DEBUG>> " << key << ":" << value;
        QJsonValue jsonValue = value.toJsonValue();
        settingsJson.insert(key,jsonValue);
    }

    QJsonDocument *jsonSettingsDocument = new QJsonDocument(settingsJson);

    jsonSettingsDocument->setObject(settingsJson);
    return jsonSettingsDocument->toJson();
}

/*!
 * \brief JsonFormatter::onLoginRequest  This function is used to initiate the process of authenticating login for the server.
 * \param username
 * \param password
 */
void JsonFormatter::onLoginRequest(QString username, QString password){
    qDebug() << "DEBUG>>  Login Request Signal Received";

    //send to Sri's void sendAuthRequest(QString password)


}

