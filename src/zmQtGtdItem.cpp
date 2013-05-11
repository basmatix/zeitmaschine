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

}

zmQtGtdItem::zmQtGtdItem( ItemRole role, const QString &title )
    : m_parentItem      ( NULL )
    , m_childItems      ()
    , m_creationTime    ( 0 )
    , m_model           ( NULL )
    , m_uid             ( "" )
    , m_text            ( title )
{

}

zmQtGtdItem::zmQtGtdItem( ItemRole role, zmQtGtdModel *model, const std::string &uid )
    : m_parentItem      ( NULL )
    , m_childItems      ()
    , m_creationTime    ( 0 )
    , m_model           ( model )
    , m_uid             ( uid )
    , m_text            ( "" )
{
    m_creationTime = m_model->getCreationTime( uid );

    m_text = QString().sprintf("%ld", m_creationTime );

    //todosetFlags( flags() | Qt::ItemIsEditable );
}
