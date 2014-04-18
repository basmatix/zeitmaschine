/// -*- coding: utf-8 -*-
///
/// file: zmQtUi.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMWIDGETITEMMAP_H
#define ZMWIDGETITEMMAP_H

#include <mm/zmTrace.h>
#include <QtCore/QMap>
#include <string>

class zmQTreeWidgetItem;
class QTreeWidgetItem;

class zmWidgetItemMap
{
    QMap< std::string, zmQTreeWidgetItem * > m_thing_lwitem_map;
    QMap< zmQTreeWidgetItem *, std::string > m_lwitem_thing_map;

public:

    zmWidgetItemMap()
        : m_thing_lwitem_map()
        , m_lwitem_thing_map()
    {

    }

    void add( const std::string uid, zmQTreeWidgetItem *item )
    {
        m_lwitem_thing_map[ item ] = uid;
        m_thing_lwitem_map[ uid ] = item;
    }

    void erase( const std::string uid, QTreeWidgetItem *item )
    {
        QMap< std::string, zmQTreeWidgetItem * >::iterator l_thingToErase =
                m_thing_lwitem_map.find( uid );
        QMap< zmQTreeWidgetItem *, std::string >::iterator l_itemToErase =
                m_lwitem_thing_map.find( (zmQTreeWidgetItem*)item );
        trace_assert_h( l_thingToErase != m_thing_lwitem_map.end() );
        trace_assert_h( l_itemToErase != m_lwitem_thing_map.end() );

        trace_assert_h( l_thingToErase.value() == (zmQTreeWidgetItem*)item );
        trace_assert_h( l_itemToErase.value() == uid );

        m_thing_lwitem_map.erase( l_thingToErase );

        m_lwitem_thing_map.erase( l_itemToErase );
    }

    bool contains( QTreeWidgetItem *item ) const
    {
        return m_lwitem_thing_map.find((zmQTreeWidgetItem*)item) != m_lwitem_thing_map.end();
    }

    bool contains( zmQTreeWidgetItem *item ) const
    {
        return m_lwitem_thing_map.find(item) != m_lwitem_thing_map.end();
    }

    zmQTreeWidgetItem * get( const std::string uid )
    {
        QMap< std::string, zmQTreeWidgetItem * >::iterator l_it =
                m_thing_lwitem_map.find( uid );
        trace_assert_h( l_it != m_thing_lwitem_map.end() );
        return l_it.value();
    }

    std::string get( QTreeWidgetItem *item ) const
    {
        QMap< zmQTreeWidgetItem *, std::string >::const_iterator l_it =
                m_lwitem_thing_map.find( (zmQTreeWidgetItem*)item );
        trace_assert_h( l_it != m_lwitem_thing_map.end() );
        return l_it.value();
    }

    const std::string & get( zmQTreeWidgetItem *item ) const
    {
        QMap< zmQTreeWidgetItem *, std::string >::const_iterator l_it =
                m_lwitem_thing_map.find( item );
        trace_assert_h( l_it != m_lwitem_thing_map.end() );
        return l_it.value();
    }

    QMap< std::string, zmQTreeWidgetItem * >::iterator begin()
    {
        return m_thing_lwitem_map.begin();
    }

    QMap< std::string, zmQTreeWidgetItem * >::iterator end()
    {
        return m_thing_lwitem_map.end();
    }

};

#endif
