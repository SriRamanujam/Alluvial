
#include "../Settings/jsonformatter.h"
#include "settings_storage.h"
#include <QDebug>

/*!
 * \brief Settings_storing::setValue This function is a wrapper around the
 * QSettings:setValue(const QString & key, const QVariant & value) function.  It is created as a wrapper in order to expose
 * the functionality of saving user's settings persistently.
 * \param key A property/key to reference the value saved by the user
 * \param value A value that will be saved in the user's config file.
 */
void Settings_storing::setValue(const QString & key, const QVariant & value){
    QSettings::setValue(key, value);
}

/*!
 * \brief Settings_storing::value This function is a wrapper around the QSettings::value(const QString &key, const QVariant & defaultValue)
 * function.  It is created as a wrapper this way to expose the functionality of the QSettings module to the .qml files which need to
 * display or output settings.
 * \param key The property to look for in the config file
 * \param defaultValue An optional default value to return if the property searched for doesn't exist
 * \return The value returned by a call to QSettings::value(key, defaultvalue).  If QSettings does not
 * find the setting, it will return defaultValue parameter.
 */
QVariant Settings_storing::value(const QString &key, const QVariant & defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

/*!
 * \brief Settings_storing::contains This function is a wrapper around the QSettings::contain(const QString &key)
 * function.  It is created as a wrapper this way to expose the functionality of the QSettings module to the .qml files which need to
 * check for the existence of user settings.
 * \param key
 * \return Returns a boolean value, true if the key exists, false if the key does not exist
 */
bool Settings_storing::contains(const QString &key) const{
    return QSettings::contains(key);
}
/*!
 * \brief Settings_storing::sync This function is a wrapper around the QSettings::sync() function.  It is created as a
 * wrapper to expose the functionality of the QSettings module to the .qml files which may
 * need to force the saving of the user's settings before they are lost.
 *
 * This function also emits a settingsSaved() signal to alert
 * the eapplication that it should send any new/modified settings to the server.
 */
void Settings_storing::sync(){
    QSettings::sync();
    emit settingsSaved();
}
/*!
 * \brief Settings_storing::getAllKeys This function is a wrapper around the QSettings::contain(const QString &key)
 * function.  This function is used to get a QStringList of all property saved in the program's corresponding config file.
 *
 * Note: No values can be obtained by using this function.
 * \return Returns a QStringList of every key found in the program's config file.
 */
QStringList Settings_storing::getAllKeys(){
    return QSettings::allKeys();
}

/*!
 * \brief Settings_storing::settingsSaved This function will get a Json formatted version of the settings file and
 * send it to the server.
 */
void Settings_storing::settingsSaved(){
    JsonFormatter *jsonformatter = new JsonFormatter();
    QByteArray jsonByteSettings = jsonformatter->getJSONSettings();
    QString jsonString(jsonByteSettings);


    /* testing output of JsonFormatter
     *
     */
/*
    //STATUS:  unsure -- output is
    // output bytearray from jsonformatter->getJSONSettings() to file
    QFile tmpfile("json_bytearray_output.txt");
    tmpfile.open(QIODevice::ReadWrite);
    if(!tmpfile.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "tmpFile Output:" << "failed";
    }
    //QDataStream in();
    qint64 bytes = tmpfile.write(jsonString.toUtf8());

    qDebug() << "Byte Written to the file:" << bytes;
    qDebug() << "tmpFile Output:" << tmpfile.readAll();
    tmpfile.close();

    // output json from jsonformatter->getJsonSettings to file
    //QJsonDocument tmpJsonDoc = QJsonDocument::fromBinaryData(jsonByteSettings, QJsonDocument::DataValidation::Validate);
    QJsonDocument tmpJsonDoc = QJsonDocument::fromRawData(jsonString.toUtf8(), jsonByteSettings.length()+1, QJsonDocument::DataValidation::Validate);
    QFile tmpfileJson("json_formatted_output.txt");
    tmpfileJson.open(QIODevice::ReadWrite);
    tmpfileJson.write(tmpJsonDoc.toJson());
    tmpfileJson.close();
    */
    /*
     *Testing Output finished
     */


}
