#ifndef SETTINGS_STORAGE
#define SETTINGS_STORAGE

#include <QObject>
#include <QSettings>
#include <iostream>
#include <fstream>
#include <QFile>
#include <QCoreApplication>
//#include "jsonformatter.h"


/*!
 \class Settings_storing

 \brief The Settings_storing class

 This class is a wrapper around the QSettings class, develooped by Qt
  This particular class will perform ________
*/
class Settings_storing:public QSettings{
    Q_OBJECT

public:
    explicit Settings_storing(QObject*parent=0): QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                                           QCoreApplication::instance()->organizationName(),
                                                           QCoreApplication::instance()->applicationName(),
                                                           parent){}

    //Q_INVOKABLE Macro which will expose this setValue function to qml elements
    //This is used to set the value for the specified key
    Q_INVOKABLE void setValue(const QString & key, const QVariant & value);
    //Q_INVOKABLE Macro which will expose this value function to qml elements
    //This is used to return the value of the specified key
    Q_INVOKABLE QVariant value(const QString &key, const QVariant & defaultValue = QVariant()) const;
    Q_INVOKABLE bool contains(const QString &key) const;
    /*
    Q_INVOKABLE Macro which will expose this sync() function to qml elements
    This is used to force/ensure that the settings values will be saved.  It also emits the signal
    which will start the json document creation process.
    */
    Q_INVOKABLE void sync();
    Q_INVOKABLE QStringList getAllKeys();


signals:


public slots:
     void settingsSaved();



private:
    QSettings client_settings;

};

Q_DECLARE_METATYPE(Settings_storing*)

#endif // SETTINGS_STORAGE
