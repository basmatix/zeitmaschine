/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "zmQtGtdModel.h"

#include <zm/zmGtdModel.h>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>

zmQtGtdModel::zmQtGtdModel()
    : m_gtd_model   ()
    , m_rootItem    ( NULL )
    //, m_liToday     ( NULL )
    , m_liInbox     ( NULL )
    , m_liProjects  ( NULL )
    , m_liKnowledge ( NULL )
    , m_liDone      ( NULL )
    , m_widget_item_mapper()
{
    QList< QVariant > l_rootData;
    l_rootData << "Title" << "Summary";
    m_rootItem = new zmQtGtdItem( zmQtGtdItem::ROOT );
}

zmQtGtdModel::~zmQtGtdModel()
{
    delete m_rootItem;
}

void zmQtGtdModel::populate()
{
    //m_liToday = new zmQtGtdItem( QList< QVariant >() << "TODAY", m_rootItem );
    //m_rootItem->appendChild( m_liToday );

    m_liInbox = new zmQtGtdItem( zmQtGtdItem::FOLDER, "INBOX" );
    m_rootItem->appendChild( m_liInbox );

    m_liProjects = new zmQtGtdItem( zmQtGtdItem::FOLDER, "PROJECTS" );
    m_rootItem->appendChild( m_liProjects );

    m_liKnowledge = new zmQtGtdItem( zmQtGtdItem::FOLDER, "KNOWLEDGE" );
    m_rootItem->appendChild( m_liKnowledge );

    m_liDone = new zmQtGtdItem( zmQtGtdItem::FOLDER, "DONE" );
    m_rootItem->appendChild( m_liDone );

    //zmQtGtdItem *test = new zmQtGtdItem( QList< QVariant >() << "TEST", m_liToday );
    //m_liToday->appendChild( test );
    /// enforce list filling order by now..

    BOOST_FOREACH( const std::string& p, getProjectItems( false, false ) )
    {
        addListItem( p );
    }

    BOOST_FOREACH( const std::string& t, getTaskItems( true, false ) )
    {
        addListItem( t );
    }

    BOOST_FOREACH( const std::string& i, getInboxItems( false ) )
    {
        addListItem( i );
    }
}

void zmQtGtdModel::autoExpand( QTreeView *treeView )
{
    //treeView->setExpanded();
}

void zmQtGtdModel::addListItem( const std::string uid )
{
    zmQtGtdItem *l_item = new zmQtGtdItem( zmQtGtdItem::GTD_ITEM, this, uid );

    //todo l_item->decorate();

    //tracemessage( "adding item %s / %d to list (%s)",
    //              uid.c_str(),
    //              l_creationTime,
    //              m_model.getCaption( uid ).toAscii().constData()  );

    if( isDone( uid ) )
    {
        m_liDone->appendChild( l_item );
    }

    else if( isInboxItem( uid ) )
    {
        m_liInbox->appendChild( l_item );
        //todo m_liInbox->sortChildren( 0, Qt::AscendingOrder );
        //todo m_liInbox->setText( 0, QString().sprintf("INBOX (%d)", m_liInbox->childCount()) );
    }

    else if( isProjectItem( uid, true ) )
    {
        m_liProjects->appendChild( l_item );
        //todo m_liProjects->setText( 0, QString().sprintf("PROJECTS (%d)", m_liProjects->childCount()) );
    }

    else if( isTaskItem( uid, false ) )
    {
        std::string l_parentProject = getParentProject( uid );

        // todo: make sure l_project exists
        zmQtGtdItem *l_project = m_widget_item_mapper.get(l_parentProject);

        l_project->appendChild( l_item );
    }

    m_widget_item_mapper.add( uid, l_item );
}

Qt::ItemFlags zmQtGtdModel::flags( const QModelIndex &index ) const
{
    if (!index.isValid())
    {
        return 0;
    }

    Qt::ItemFlags l_return = Qt::NoItemFlags;
    l_return |= Qt::ItemIsSelectable;
    l_return |= Qt::ItemIsEnabled;
    l_return |= Qt::ItemIsDragEnabled;
    l_return |= Qt::ItemIsDropEnabled;
    l_return |= Qt::ItemIsEditable;

    return l_return;
}

QVariant zmQtGtdModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        return m_rootItem->data( section );
    }

    return QVariant();
}

QModelIndex zmQtGtdModel::index( int row, int column, const QModelIndex &parent ) const
{
    if( !hasIndex(row, column, parent) )
    {
        return QModelIndex();
    }

    zmQtGtdItem *l_parentItem;

    if( !parent.isValid() )
    {
        l_parentItem = m_rootItem;
    }
    else
    {
        l_parentItem = static_cast<zmQtGtdItem*>(parent.internalPointer());
    }

    zmQtGtdItem *l_childItem = l_parentItem->child(row);
    if( l_childItem == NULL )
    {
        return QModelIndex();
    }

    return createIndex(row, column, l_childItem);
}

QModelIndex zmQtGtdModel::parent( const QModelIndex &index ) const
{
    if( !index.isValid() )
    {
        return QModelIndex();
    }

    zmQtGtdItem *l_childItem = static_cast< zmQtGtdItem * >( index.internalPointer() );
    zmQtGtdItem *l_parentItem = l_childItem->parent();

    if( l_parentItem == m_rootItem )
    {
        return QModelIndex();
    }

    return createIndex(l_parentItem->row(), 0, l_parentItem);
}

int zmQtGtdModel::rowCount( const QModelIndex &parent ) const
{
    if( parent.column() > 0 )
    {
        return 0;
    }

    zmQtGtdItem *l_parentItem;

    if (!parent.isValid())
    {
        l_parentItem = m_rootItem;
    }
    else
    {
        l_parentItem = static_cast< zmQtGtdItem * >(parent.internalPointer());
    }

    return l_parentItem->childCount();
}

int zmQtGtdModel::columnCount( const QModelIndex &parent ) const
{
    if( parent.isValid() )
    {
        return static_cast< zmQtGtdItem* >( parent.internalPointer() )->columnCount();
    }
    else
    {
        return m_rootItem->columnCount();
    }
}

QVariant zmQtGtdModel::data( const QModelIndex &index, int role ) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    zmQtGtdItem *l_item = static_cast< zmQtGtdItem * >(index.internalPointer());

    switch( role )
    {
    case Qt::DisplayRole:
    {
        return l_item->data( index.column() );
    }
    case Qt::TextColorRole: //ForegroundRole
        return QVariant( l_item->color( index.column() ) );
    case Qt::SizeHintRole:
    case Qt::ToolTipRole:
    case Qt::DecorationRole:
    case Qt::FontRole:
    case Qt::TextAlignmentRole:
    case Qt::BackgroundRole:
    case Qt::CheckStateRole:

    {
        return QVariant();
        break;
    }
    default:
        return QVariant();
        break;
    }
}
