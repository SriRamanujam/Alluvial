#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <QObject>

/*!
 * \brief The DataObject class Used for displaying playlist songs
 *
 *
 * A utility class needed for displaying songs.
 * Gets song names from the calling class, and creates list elements for displaying each playlist_item stored in a playlist.
 */
class DataObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )


public:
    DataObject( QObject * parent = 0 );
    DataObject( const QString &_name, QObject * parent=0 );

    QString name() const;
    void setName(const QString &);

signals:
    void nameChanged();


private:
    QString m_name;
};


#endif // DATAOBJECT_H
