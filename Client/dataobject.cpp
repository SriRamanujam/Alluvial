#include "dataobject.h"
#include <QDebug>

DataObject::DataObject( QObject * parent )
    : QObject( parent )
{
}

DataObject::DataObject( const QString &_name, QObject * parent )
    : QObject( parent )
    , m_name( _name )
{
}


QString DataObject::name() const
{
    return m_name;
}

void DataObject::setName(const QString &name)
{
    if ( name != m_name ) {
        m_name = name;
        emit nameChanged();
    }
}
