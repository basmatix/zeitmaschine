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
{
    assert( role == zmQtGtdItem::GTD_ITEM );

    m_creationTime = m_model->getCreationTime( uid );

    // m_text = QString().sprintf("%ld", m_creationTime );

    //todo setFlags( flags() | Qt::ItemIsEditable );
}
