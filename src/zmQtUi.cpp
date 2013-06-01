/// -*- coding: utf-8 -*-
///
/// file: zmQtUi.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "ui_zeitmaschine.h"
#include "zmQtUi.h"

#include "zmQtGtdModel.h"
#include <mm/zmOsal.h>

#include <mm/zmTrace.h>

#include <map>
#include <QtGui/QMainWindow>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QDataStream>
#include <QtCore/QMutexLocker>
#include <QtCore/QTime>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

#include <boost/foreach.hpp>

#include <iostream>
#include <assert.h>


zmQtUi::zmQtUi( QWidget *parent )
    : QMainWindow          ( parent )
    , m_ui                 ( new Ui_window )
    , m_model              ()
    /*
    , m_selected_thing     ( "" )
    , m_selected_twItem    ( NULL )
    , m_liToday            ()
    , m_liInbox            ()
    , m_liProjects         ()
    , m_liContexts         ()
    , m_liDone             ()
    , m_widget_item_mapper ()
*/
    , m_searchProxy         ( NULL )
{   tracemessage( __FUNCTION__ );

    m_ui->setupUi( this );

    setWindowTitle(
                QApplication::translate(
                    "window",
                    "zeitmaschine - built on "__TIMESTAMP__,
                    0, QApplication::UnicodeUTF8 ));

    if( QDir( QDir::currentPath() + QDir::separator() + "zm-local").exists() )
    {
        m_model.setLocalFolder( QDir::currentPath() + QDir::separator() + "zm-local" );
    }
    else
    {
        m_model.setLocalFolder( QDir::homePath() + QDir::separator() + "zm-local" );
    }

    if( !m_model.hasUsedUsername() )
    {
        bool ok;
        QString text = QInputDialog::getText( this, tr("QInputDialog::getText()"),
                                                tr("user name:"), QLineEdit::Normal,
                                                QDir::home().dirName(), &ok);
        m_model.setUsedUsername( text );
    }
    if( !m_model.hasUsedHostname() )
    {
        bool ok;
        QString text = QInputDialog::getText( this, tr("QInputDialog::getText()"),
                                                tr("machine name:"), QLineEdit::Normal,
                                                zm::osal::getHostName().c_str(), &ok);
        m_model.setUsedHostname( text );
    }

    m_model.initialize();

    if(0)
    {
    //m_searchProxy = new MySortFilterProxyModel( this );
    //m_searchProxy->setSourceModel( &m_model );
        //m_ui->tvMaster->setModel( m_searchProxy );
    }
    else
    {
        m_ui->tvMaster->setModel( &m_model );
        m_model.autoExpand( m_ui->tvMaster );
    }
/*
    m_liToday = new QTreeWidgetItem();
    m_liToday->setText( 0, "TODAY");
    m_ui->twMaster->addTopLevelItem( m_liToday );
    m_liToday->setExpanded( true );
    // should be an independent object of course
    QBrush b = m_liToday->foreground(0);
    b.setColor( Qt::darkBlue );
    m_liToday->setForeground( 0, b );
    m_liToday->setBackgroundColor( 0, Qt::lightGray );

    m_liInbox = new QTreeWidgetItem();
    m_liInbox->setText( 0, "INBOX");
    m_ui->twMaster->addTopLevelItem( m_liInbox );
    m_liInbox->setExpanded( true );
    m_liInbox->setForeground( 0, b );
    m_liInbox->setBackgroundColor( 0, Qt::lightGray );

    m_liProjects = new QTreeWidgetItem();
    m_liProjects->setText( 0, "PROJECTS");
    m_ui->twMaster->addTopLevelItem( m_liProjects );
    m_liProjects->setExpanded( true );
    m_liProjects->setForeground( 0, b );
    m_liProjects->setBackgroundColor( 0, Qt::lightGray );

    m_liContexts = new QTreeWidgetItem();
    m_liContexts->setText( 0, "CONTEXT");
    m_ui->twMaster->addTopLevelItem( m_liContexts );
    m_liContexts->setExpanded( true );
    m_liContexts->setForeground( 0, b );
    m_liContexts->setBackgroundColor( 0, Qt::lightGray );

    m_liDone = new QTreeWidgetItem();
    m_liDone->setText( 0, "DONE");
    m_ui->twMaster->addTopLevelItem( m_liDone );
    m_liDone->setExpanded( false );
    m_liDone->setForeground( 0, b );
    m_liDone->setBackgroundColor( 0, Qt::lightGray );
*/
    updateUi();
}

zmQtUi::~zmQtUi()
{
    delete m_ui;
}

void zmQtUi::addListItem( const std::string uid )
{
//    zmQTreeWidgetItem *l_item = new zmQTreeWidgetItem( m_model, uid );
/*
    l_item->decorate();

    //tracemessage( "adding item %s / %d to list (%s)",
    //              uid.c_str(),
    //              l_creationTime,
    //              m_model.getCaption( uid ).toAscii().constData()  );

    if( m_model.isDone( uid ) )
    {
        m_liDone->addChild( l_item );
    }

    else if( m_model.isInboxItem( uid ) )
    {
        m_liInbox->addChild( l_item );
        m_liInbox->sortChildren(0,Qt::AscendingOrder);
        m_liInbox->setText( 0, QString().sprintf("INBOX (%d)", m_liInbox->childCount()) );
    }
    else if( m_model.isProjectItem( uid, true ) )
    {
        m_liProjects->addChild( l_item );
        m_liProjects->setText( 0, QString().sprintf("PROJECTS (%d)", m_liProjects->childCount()) );
    }

    else if( m_model.isTaskItem( uid, false ) )
    {
        std::string l_parentProject = m_model.getParentProject( uid );

        // todo: make sure l_project exists
        zmQTreeWidgetItem *l_project = m_widget_item_mapper.get(l_parentProject);

        l_project->addChild( l_item );
    }

    m_widget_item_mapper.add( uid, l_item );
*/
}

void zmQtUi::updateUi()
{
    /// enforce list filling order by now..

    BOOST_FOREACH( const std::string& p, m_model.getProjectItems( false, false ) )
    {
        addListItem( p );
    }

    BOOST_FOREACH( const std::string& t, m_model.getTaskItems( true, false ) )
    {
        addListItem( t );
    }

    BOOST_FOREACH( const std::string& i, m_model.getInboxItems( false ) )
    {
        addListItem( i );
    }
}

void zmQtUi::closeEvent( QCloseEvent *event )
{   tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    m_model.sync();
}

void zmQtUi::unselect()
{
    /*
    m_selected_thing = "";
    m_selected_twItem = NULL;
    m_ui->twMaster->unselect();
    */
}

void zmQtUi::createInboxItemFromUiElements()
{
    std::string l_item_uid = m_model.createNewInboxItem( m_ui->leCommand->text() );

    m_model.setNote( l_item_uid, m_ui->teNotes->toPlainText() );

    addListItem( l_item_uid );

    m_ui->leCommand->setText("");
}

void zmQtUi::on_pbAddInboxItem_clicked()
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    createInboxItemFromUiElements();
}

void zmQtUi::on_leCommand_returnPressed()
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    createInboxItemFromUiElements();
}
/*
void zmQtUi::on_twMaster_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous )
{   //tracemessage( __FUNCTION__ );

    if( m_widget_item_mapper.contains( previous ) )
    {
        /// if there has been selected - save it's note
        std::string l_previous_thing = m_widget_item_mapper.get( previous );

        //assert( l_previous_thing == m_selected_thing );
        if( previous == m_selected_twItem )
        {
            m_model.setNote( l_previous_thing, m_ui->teNotes->toPlainText() );
        }
    }

    m_ui->teNotes->setText( "" );

    if( m_widget_item_mapper.contains( current ) )
    {

        m_selected_twItem = (zmQTreeWidgetItem *)current;
        m_selected_thing = m_widget_item_mapper.get( m_selected_twItem );
        tracemessage( "clicked on item %s (%s)",
                      m_selected_thing.c_str(),
                      m_model.getCaption( m_selected_thing ).toAscii().constData()  );
    }
    else
    {
        m_selected_thing = "";
        m_selected_twItem = NULL;
    }

    m_ui->teNotes->setText( m_model.getNote( m_selected_thing ) );
}

void zmQtUi::on_twMaster_itemChanged( QTreeWidgetItem *item )
{   //tracemessage( __FUNCTION__ );

    if( m_widget_item_mapper.contains( item ) )
    {
        std::string l_thing = m_widget_item_mapper.get( item );
        QString l_new_caption = item->text(0);

        tracemessage( "changing item text from '%s' to '%s'",
                      m_model.getCaption( l_thing ).toAscii().constData(),
                      l_new_caption.toAscii().constData() );
        m_model.setCaption( l_thing, l_new_caption );

    }
}

void zmQtUi::on_twMaster_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target )
{   //tracemessage( __FUNCTION__ );

    if( !m_widget_item_mapper.contains( item ) )
    {
        return;
    }
    if( !m_widget_item_mapper.contains( target ) )
    {
        return;
    }

    std::string l_source = m_widget_item_mapper.get( item );
    std::string l_target = m_widget_item_mapper.get( target );

    tracemessage( "dragged '%s' to '%s'",
                  m_model.getCaption( l_source ).toAscii().constData(),
                  m_model.getCaption( l_target ).toAscii().constData() );

    if( m_model.isInboxItem( l_source )
     && m_model.isProjectItem( l_target, false ) )
    {
        m_model.registerItemAsTask( l_source, l_target );

        // NOTE: this is black magic - don't touch! why does m_selected_twItem
        //       get set to NULL on removeChild()?!
        zmQTreeWidgetItem *l_uselessCopy( (zmQTreeWidgetItem *)item );
        QTreeWidgetItem *l_groupingItem( item->parent() );
        l_groupingItem->removeChild( l_uselessCopy );
        l_uselessCopy->decorate();
        target->addChild( l_uselessCopy );
    }
}
*/
void zmQtUi::on_pbClose_clicked()
{   tracemessage( __FUNCTION__ );

    //QMutexLocker monitor( &m_mutex );
    /*
    if( m_selected_thing == "" ) return;

    tracemessage( "set item to DONE: %s (%s)",
                  m_selected_thing.c_str(),
                  m_model.getCaption( m_selected_thing ).toAscii().constData()  );

    m_model.setDone( m_selected_thing );

    // NOTE: this is black magic - don't touch! why does m_selected_twItem
    //       get set to NULL on removeChild()?!

    zmQTreeWidgetItem *l_uselessCopy( m_selected_twItem );
    QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
    l_groupingItem->removeChild( l_uselessCopy );
    l_uselessCopy->decorate();
    m_liDone->addChild( l_uselessCopy );

    unselect();
    */
}

void zmQtUi::on_pbDelete_clicked()
{   tracemessage( __FUNCTION__ );
    /*
    if( m_selected_thing == "" ) return;

    tracemessage( "erase item: %s (%s)",
                  m_selected_thing.c_str(),
                  m_model.getCaption( m_selected_thing ).toAscii().constData()  );

    m_model.eraseItem( m_selected_thing );

    m_widget_item_mapper.erase( m_selected_thing, m_selected_twItem );

    // dont alter m_selected_thing or m_selected_twItem after this
    // deletion since they get set there synchronously
    delete m_selected_twItem;
*/
    unselect();
}


void zmQtUi::on_pbPlusOne_clicked()
{   tracemessage( __FUNCTION__ );
/*
    if( m_selected_thing == "" ) return;

    tracemessage( "plus one item %s (%s)",
                  m_selected_thing.c_str(),
                  m_model.getCaption( m_selected_thing ).toAscii().constData()  );
    m_model.plusOne( m_selected_thing );
*/
//    m_selected_twItem->decorate();
}

void zmQtUi::on_pbMakeProject_clicked()
{   tracemessage( __FUNCTION__ );

    QString l_project_name = m_ui->leCommand->text();
/*
    /// if no name for a new item has been given cast an inbox item
    if( l_project_name == "" )
    {
        if( m_selected_thing == "" )
        {
            // todo: error
            assert(false);
        }
        assert( m_widget_item_mapper.get( m_selected_twItem ) == m_selected_thing );
        assert( m_widget_item_mapper.get( m_selected_thing ) == m_selected_twItem );
        if( m_model.isInboxItem( m_selected_thing ))
        {
            tracemessage( "turn item into project: %s (%s)",
                          m_selected_thing.c_str(),
                          m_model.getCaption( m_selected_thing ).toAscii().constData()  );

            m_model.castToProject( m_selected_thing );

            // NOTE: this is black magic - don't touch! why does m_selected_twItem
            //       get set to NULL on removeChild()?!
            zmQTreeWidgetItem *l_uselessCopy( m_selected_twItem );
            QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
            l_groupingItem->removeChild( l_uselessCopy );
            l_uselessCopy->decorate();
            m_liProjects->addChild( l_uselessCopy );

            m_selected_thing = "";
            m_selected_twItem = NULL;
        }
        else
        {
            // todo: error
            assert(false);
        }
    }
    else
    {
        tracemessage( "create project '%s' from magic line",
                      l_project_name.toAscii().constData() );

        std::string l_item_uid = m_model.createProject( l_project_name );

        addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }
        */
}

void zmQtUi::on_pbMakeNextAction_clicked()
{
    /*
    if( m_selected_thing == "" )
    {
        // todo: error
        assert(false);
    }
    assert( m_widget_item_mapper.get( m_selected_twItem ) == m_selected_thing );
    assert( m_widget_item_mapper.get( m_selected_thing ) == m_selected_twItem );

    if( m_model.isTaskItem( m_selected_thing, false ) )
    {
        std::string l_parentProject = m_model.getParentProject( m_selected_thing );

        std::string l_formerNextTask = m_model.getNextTask( l_parentProject );

        tracemessage( "define item %s (%s) to be next item for %s (%s)",
                      m_selected_thing.c_str(),
                      m_model.getCaption( m_selected_thing ).toAscii().constData(),
                      l_parentProject.c_str(),
                      m_model.getCaption( l_parentProject ).toAscii().constData() );

        m_model.setNextTask( l_parentProject, m_selected_thing );

        m_selected_twItem->decorate();
        m_widget_item_mapper.get( l_parentProject )->decorate();

        if( l_formerNextTask != "" )
        {
            m_widget_item_mapper.get( l_formerNextTask )->decorate();
        }
        unselect();
    }
    else
    {
        // todo: error
        assert(false);
    }
    */
}

void zmQtUi::on_pbMakeAction_clicked()
{   tracemessage( __FUNCTION__ );

    QString l_project_name = m_ui->leCommand->text();
/*
    /// if no name for a new item has been given cast an inbox item
    if( l_project_name == "" )
    {
        if( m_selected_thing == "" )
        {
            // todo: error
            assert(false);
        }
        assert( m_widget_item_mapper.get( m_selected_twItem ) == m_selected_thing );
        assert( m_widget_item_mapper.get( m_selected_thing ) == m_selected_twItem );
        if( m_model.isInboxItem( m_selected_thing ))
        {
            tracemessage( "turn item into project: %s (%s)",
                          m_selected_thing.c_str(),
                          m_model.getCaption( m_selected_thing ).toAscii().constData()  );

            m_model.castToProject( m_selected_thing );

            // NOTE: this is black magic - don't touch! why does m_selected_twItem
            //       get set to NULL on removeChild()?!
            zmQTreeWidgetItem *l_uselessCopy( m_selected_twItem );
            QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
            l_groupingItem->removeChild( l_uselessCopy );
            l_uselessCopy->decorate();
            m_liProjects->addChild( l_uselessCopy );

            m_selected_thing = "";
            m_selected_twItem = NULL;
        }
        else
        {
            // todo: error
            assert(false);
        }
    }
    else
    {
        tracemessage( "create project '%s' from magic line",
                      l_project_name.toAscii().constData() );

        std::string l_item_uid = m_model.createProject( l_project_name );

        addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }
    */
}

void zmQtUi::on_pbOrder_clicked()
{
    std::string l_taskUid = m_model.orderATask();

    if( l_taskUid == "" )
    {
        return;
    }

    QString l_taskCaption = m_model.getCaption( l_taskUid );
    QMessageBox::information( this, "just do it!", l_taskCaption );
}

void zmQtUi::on_leCommand_textChanged ( const QString & text )
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt
    m_searchProxy->setSearchString( text );
/*
    QString l_search_string = text.toLower();
    for( QMap< std::string, zmQTreeWidgetItem * >::iterator
         i  = m_widget_item_mapper.begin();
         i != m_widget_item_mapper.end(); ++i )
    {
        bool l_search_string_matches = m_model.itemContentMatchesString(
                    i.key(), l_search_string );
        i.value()->setHidden( ! l_search_string_matches );
    }

    /// eventually current note save
    if( m_selected_thing != "" )
    {
        m_model.setNote( m_selected_thing, m_ui->teNotes->toPlainText() );
        m_ui->teNotes->setText( "" );
        assert( m_selected_twItem == m_widget_item_mapper.get(m_selected_thing) );
    }
    unselect();
    */
}
