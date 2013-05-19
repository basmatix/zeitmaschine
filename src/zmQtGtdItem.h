/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdItem.h
///
///

#ifndef ZMQTGTDITEM_H
#define ZMQTGTDITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>

#include <ctime>

class zmQtGtdModel;

class zmQtGtdItem
{
    zmQtGtdItem            *m_parentItem;
    QList< zmQtGtdItem * >  m_childItems;
    std::time_t             m_creationTime;
    zmQtGtdModel           *m_model;
    std::string             m_uid;
    QString                 m_text;
    int                     m_sortValue;

    zmQtGtdItem( const zmQtGtdItem & );
    zmQtGtdItem & operator= ( const zmQtGtdItem & );

public:

    enum ItemRole
    {
        ROOT,
        FOLDER,
        GTD_ITEM
    };

    zmQtGtdItem( ItemRole role );

    zmQtGtdItem( ItemRole role, const QString &title );

    zmQtGtdItem( ItemRole role, zmQtGtdModel *model, const std::string &uid );

    virtual ~zmQtGtdItem()
    {
        qDeleteAll( m_childItems );
    }

    void setParent( zmQtGtdItem *item )
    {
        m_parentItem = item;
    }

    void appendChild( zmQtGtdItem *item )
    {
        item->setParent( this );
        for(QList< zmQtGtdItem * >::iterator
            i  = m_childItems.begin();
            i != m_childItems.end(); ++i )
        {
            if( *item < **i )
            {
                m_childItems.insert( i, item );
                return;
            }
        }

        m_childItems.append( item );
    }

    bool operator< ( const zmQtGtdItem &other )
    {
        return m_sortValue < other.m_sortValue;
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
        return 1; //todo
        //return m_itemData.count();
    }

    QVariant data( int column ) const;

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
