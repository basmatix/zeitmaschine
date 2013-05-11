/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdItem.h
///
///

#ifndef ZMQTGTDITEM_H
#define ZMQTGTDITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>

class zmQtGtdItem
{
    zmQtGtdItem            *m_parentItem;
    QList< QVariant >       m_itemData;
    QList< zmQtGtdItem * >  m_childItems;

    zmQtGtdItem( const zmQtGtdItem & );
    zmQtGtdItem & operator= ( const zmQtGtdItem & );

public:

    zmQtGtdItem( const QList< QVariant > &data, zmQtGtdItem *parent = NULL )
        : m_parentItem  ( parent )
        , m_itemData    ( data )
        , m_childItems  ()
    {    }

    virtual ~zmQtGtdItem()
    {
        qDeleteAll( m_childItems );
    }

    void appendChild( zmQtGtdItem *item )
    {
        m_childItems.append( item );
    }

    zmQtGtdItem *child( int row )
    {
        return m_childItems.value( row );
    }

    int childCount() const
    {
        return m_childItems.count();
    }

    int columnCount() const
    {
        return m_itemData.count();
    }

    QVariant data( int column ) const
    {
        return m_itemData.value( column );
    }

    int row() const
    {
        if( m_parentItem == NULL )
        {
            return 0;
        }

        return m_parentItem->m_childItems.indexOf( const_cast< zmQtGtdItem * >(this) );
    }

    zmQtGtdItem *parent()
    {
        return m_parentItem;
    }
};

#endif
