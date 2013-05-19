/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdItem.cpp
///
///

#include "zmQtGtdItem.h"
#include "zmQtGtdModel.h"

zmQtGtdItem::zmQtGtdItem( ItemRole role )
    : m_parentItem      ( NULL )
    , m_childItems      ()
    , m_creationTime    ( 0 )
    , m_model           ( NULL )
    , m_uid             ( "" )
    , m_text            ( "" )
    , m_sortValue       ( 0 )
{
    assert( role == zmQtGtdItem::ROOT );
}

zmQtGtdItem::zmQtGtdItem( ItemRole role, const QString &title )
    : m_parentItem      ( NULL )
    , m_childItems      ()
    , m_creationTime    ( 0 )
    , m_model           ( NULL )
    , m_uid             ( "" )
    , m_text            ( title )
    , m_sortValue       ( 0 )
{
    assert( role == zmQtGtdItem::FOLDER );
}

zmQtGtdItem::zmQtGtdItem( ItemRole role, zmQtGtdModel *model, const std::string &uid )
    : m_parentItem      ( NULL )
    , m_childItems      ()
    , m_creationTime    ( 0 )
    , m_model           ( model )
    , m_uid             ( uid )
    , m_text            ( "" )
    , m_sortValue       ( 0 )
{
    assert( role == zmQtGtdItem::GTD_ITEM );

    m_creationTime = m_model->getCreationTime( uid );

    m_text = m_model->getCaption( m_uid );

    m_sortValue = m_creationTime;

    //todo setFlags( flags() | Qt::ItemIsEditable );
}

QVariant zmQtGtdItem::data( int column ) const
{
    //todo
    if( column == 0 )
    {
        return m_text;
    }
    else if( column == 2 )
    {
        return m_sortValue;
    }
    else
    {
        return QVariant();
    }
    //return m_itemData.value( column );
}
