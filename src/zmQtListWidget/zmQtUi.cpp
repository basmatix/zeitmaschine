/// -*- coding: utf-8 -*-
///
/// file: zmQtUi.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "zmQtUi.h"

#include <zm/zmGtdModel.h>
#include <mm/zmOsal.h>
#include <mm/zmTrace.h>
#include "zmWidgetItemMap.h"

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

//#include <iostream>
//#include <assert.h>

#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow          ( parent )
    , m_ui                 ( new Ui_window )
    , m_model              ()
    , m_selected_thing     ( "" )
    , m_selected_twItem    ( NULL )
{   tracemessage( __FUNCTION__ );

    m_ui->setupUi( this );

    this->setWindowTitle(QApplication::translate("window", "zeitmaschine - built on "__TIMESTAMP__, 0, QApplication::UnicodeUTF8));

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

    m_model.initialize(  m_ui->twTask->invisibleRootItem() );

}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::updateGui()
{   tracemessage( __FUNCTION__ );

}

void MainWindow::closeEvent( QCloseEvent *event )
{   tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    m_model.sync();
}

void MainWindow::unselect()
{
    m_selected_thing = "";
    m_selected_twItem = NULL;
    m_ui->twTask->unselect();
}

void MainWindow::createInboxItemFromUiElements()
{
    std::string l_item_uid = m_model.createNewInboxItem( m_ui->leCommand->text() );

    m_model.setNote( l_item_uid, m_ui->teNotes->toPlainText() );

    m_model.addListItem( l_item_uid );

    m_ui->leCommand->setText("");
}

void MainWindow::on_pbAddInboxItem_clicked()
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    createInboxItemFromUiElements();
}

void MainWindow::on_leCommand_returnPressed()
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt

    createInboxItemFromUiElements();
}

void MainWindow::on_twTask_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous )
{   //tracemessage( __FUNCTION__ );

    if( m_model.m_wi_map->contains( previous ) )
    {
        /// if there has been selected - save it's note
        std::string l_previous_thing = m_model.m_wi_map->get( previous );

        //assert( l_previous_thing == m_selected_thing );
        if( previous == m_selected_twItem )
        {
            m_model.setNote( l_previous_thing, m_ui->teNotes->toPlainText() );
        }
    }

    m_ui->teNotes->setText( "" );

    if( m_model.m_wi_map->contains( current ) )
    {

        m_selected_twItem = (zmQTreeWidgetItem *)current;
        m_selected_thing = m_model.m_wi_map->get( m_selected_twItem );
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

void MainWindow::on_twTask_itemChanged( QTreeWidgetItem *item )
{   //tracemessage( __FUNCTION__ );

    if( m_model.m_wi_map->contains( item ) )
    {
        std::string l_thing = m_model.m_wi_map->get( item );
        QString l_new_caption = item->text(0);

        tracemessage( "changing item text from '%s' to '%s'",
                      m_model.getCaption( l_thing ).toAscii().constData(),
                      l_new_caption.toAscii().constData() );
        m_model.setCaption( l_thing, l_new_caption );

    }
}

void MainWindow::on_twTask_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target )
{   //tracemessage( __FUNCTION__ );

    if( !m_model.m_wi_map->contains( item ) )
    {
        return;
    }
    if( !m_model.m_wi_map->contains( target ) )
    {
        return;
    }

    std::string l_source = m_model.m_wi_map->get( item );
    std::string l_target = m_model.m_wi_map->get( target );

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

void MainWindow::on_pbClose_clicked()
{   tracemessage( __FUNCTION__ );

    //QMutexLocker monitor( &m_mutex );
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
    m_model.m_liDone->addChild( l_uselessCopy );

    unselect();
}

void MainWindow::on_pbDelete_clicked()
{   tracemessage( __FUNCTION__ );

    if( m_selected_thing == "" ) return;

    tracemessage( "erase item: %s (%s)",
                  m_selected_thing.c_str(),
                  m_model.getCaption( m_selected_thing ).toAscii().constData()  );

    m_model.eraseItem( m_selected_thing );

    m_model.m_wi_map->erase( m_selected_thing, m_selected_twItem );

    // dont alter m_selected_thing or m_selected_twItem after this
    // deletion since they get set there synchronously
    delete m_selected_twItem;

    unselect();
}


void MainWindow::on_pbPlusOne_clicked()
{   tracemessage( __FUNCTION__ );

    if( m_selected_thing == "" ) return;

    tracemessage( "plus one item %s (%s)",
                  m_selected_thing.c_str(),
                  m_model.getCaption( m_selected_thing ).toAscii().constData()  );

    m_model.plusOne( m_selected_thing );
    m_selected_twItem->decorate();
}

void MainWindow::on_pbMakeProject_clicked()
{   tracemessage( __FUNCTION__ );

    QString l_project_name = m_ui->leCommand->text();

    /// if no name for a new item has been given cast an inbox item
    if( l_project_name == "" )
    {
        if( m_selected_thing == "" )
        {
            // todo: error
            assert(false);
        }
        assert( m_model.m_wi_map->get( m_selected_twItem ) == m_selected_thing );
        assert( m_model.m_wi_map->get( m_selected_thing ) == m_selected_twItem );
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
            m_model.m_liProjects->addChild( l_uselessCopy );

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

        m_model.addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }
}

void MainWindow::on_pbMakeNextAction_clicked()
{
    if( m_selected_thing == "" )
    {
        // todo: error
        assert(false);
    }
    assert( m_model.m_wi_map->get( m_selected_twItem ) == m_selected_thing );
    assert( m_model.m_wi_map->get( m_selected_thing ) == m_selected_twItem );

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
        m_model.m_wi_map->get( l_parentProject )->decorate();

        if( l_formerNextTask != "" )
        {
            m_model.m_wi_map->get( l_formerNextTask )->decorate();
        }
        unselect();
    }
    else
    {
        // todo: error
        assert(false);
    }
}

void MainWindow::on_pbMakeAction_clicked()
{   tracemessage( __FUNCTION__ );

    QString l_project_name = m_ui->leCommand->text();

    /// if no name for a new item has been given cast an inbox item
    if( l_project_name == "" )
    {
        if( m_selected_thing == "" )
        {
            // todo: error
            assert(false);
        }
        assert( m_model.m_wi_map->get( m_selected_twItem ) == m_selected_thing );
        assert( m_model.m_wi_map->get( m_selected_thing ) == m_selected_twItem );
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
            m_model.m_liProjects->addChild( l_uselessCopy );

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

        m_model.addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }
}

void MainWindow::on_pbOrder_clicked()
{
    std::string l_taskUid = m_model.orderATask();

    if( l_taskUid == "" )
    {
        return;
    }

    QString l_taskCaption = m_model.getCaption( l_taskUid );
    QMessageBox::information( this, "just do it!", l_taskCaption );
}

void MainWindow::on_leCommand_textChanged ( const QString & text )
{   //tracemessage( __FUNCTION__ );
    // this method is being called automatically by Qt
    QString l_search_string = text.toLower();
    for( QMap< std::string, zmQTreeWidgetItem * >::iterator
         i  = m_model.m_wi_map->begin();
         i != m_model.m_wi_map->end(); ++i )
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
        assert( m_selected_twItem == m_model.m_wi_map->get(m_selected_thing) );
    }
    unselect();
}
