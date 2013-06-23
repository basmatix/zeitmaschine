/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

/// this is a workaround for a qt based bug
/// http://stackoverflow.com/questions/15191361/boostq-foreach-has-not-been-declared
#include <boost/foreach.hpp>

#include "zmQtGtdModel.h"
#include "zmWidgetItemMap.h"
#include "zmQTreeWidget.h"

#include <boost/foreach.hpp>
#include <QtGui/QTreeWidgetItem>

zmQtGtdModel::zmQtGtdModel()
    : m_gtd_model()
    , m_wi_map( NULL )
{
    m_wi_map = new zmWidgetItemMap();

}

void zmQtGtdModel::initialize( QTreeWidgetItem *root )
{
    m_gtd_model.initialize();

    m_liToday = new QTreeWidgetItem();
    m_liToday->setText( 0, "TODAY");
    m_liToday->setExpanded( true );
    // should be an independent object of course
    QBrush b = m_liToday->foreground(0);
    b.setColor( Qt::darkBlue );
    m_liToday->setForeground( 0, b );
    m_liToday->setBackgroundColor( 0, Qt::lightGray );

    m_liInbox = new QTreeWidgetItem();
    m_liInbox->setText( 0, "INBOX");
    m_liInbox->setExpanded( true );
    m_liInbox->setForeground( 0, b );
    m_liInbox->setBackgroundColor( 0, Qt::lightGray );

    m_liProjects = new QTreeWidgetItem();
    m_liProjects->setText( 0, "PROJECTS");
    m_liProjects->setExpanded( true );
    m_liProjects->setForeground( 0, b );
    m_liProjects->setBackgroundColor( 0, Qt::lightGray );

    m_liContexts = new QTreeWidgetItem();
    m_liContexts->setText( 0, "CONTEXT");
    m_liContexts->setExpanded( true );
    m_liContexts->setForeground( 0, b );
    m_liContexts->setBackgroundColor( 0, Qt::lightGray );

    m_liDone = new QTreeWidgetItem();
    m_liDone->setText( 0, "DONE");
    m_liDone->setExpanded( false );
    m_liDone->setForeground( 0, b );
    m_liDone->setBackgroundColor( 0, Qt::lightGray );

    root->addChild( m_liToday );
    root->addChild( m_liInbox );
    root->addChild( m_liProjects );
    root->addChild( m_liContexts );
    root->addChild( m_liDone );

    populate();
}

void zmQtGtdModel::addListItem( const std::string uid )
{
    zmQTreeWidgetItem *l_item = new zmQTreeWidgetItem( *this, uid );

    l_item->decorate();

    //tracemessage( "adding item %s / %d to list (%s)",
    //              uid.c_str(),
    //              l_creationTime,
    //              m_model.getCaption( uid ).toAscii().constData()  );

    if( isDone( uid ) )
    {
        m_liDone->addChild( l_item );
    }

    else if( isInboxItem( uid ) )
    {
        m_liInbox->addChild( l_item );
        m_liInbox->sortChildren(0,Qt::AscendingOrder);
        m_liInbox->setText( 0, QString().sprintf("INBOX (%d)", m_liInbox->childCount()) );
    }

    else if( isProjectItem( uid, true ) )
    {
        m_liProjects->addChild( l_item );
        m_liProjects->setText( 0, QString().sprintf("PROJECTS (%d)", m_liProjects->childCount()) );
    }

    else if( isTaskItem( uid, false ) )
    {
        std::string l_parentProject = getParentProject( uid );

        // todo: make sure l_project exists
        zmQTreeWidgetItem *l_project = m_wi_map->get(l_parentProject);

        l_project->addChild( l_item );
    }

    m_wi_map->add( uid, l_item );
}

void zmQtGtdModel::populate()
{
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

zmQtGtdModel::~zmQtGtdModel()
{
    delete m_wi_map;
}
