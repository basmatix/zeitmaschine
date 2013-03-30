/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_zeitmaschine.h"

#include "zmQtGtdModel.h"

#include "zmTrace.h"

#include <map>
#include <QtGui/QMainWindow>
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

#include <iostream>
#include <assert.h>

class QListWidgetItem;

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

private:

    Ui_window       *m_ui;

    zmQtGtdModel     m_model;

    std::string      m_selected_thing;
    QTreeWidgetItem *m_selected_twItem;

    QTreeWidgetItem *m_liToday;
    QTreeWidgetItem *m_liInbox;
    QTreeWidgetItem *m_liProjects;
    QTreeWidgetItem *m_liContexts;
    QTreeWidgetItem *m_liDone;

    class WidgetItemMapper
    {
        QMap< std::string, zmQTreeWidgetItem * > m_thing_lwitem_map;
        QMap< zmQTreeWidgetItem *, std::string > m_lwitem_thing_map;

    public:

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
            assert( l_thingToErase != m_thing_lwitem_map.end() );
            assert( l_itemToErase != m_lwitem_thing_map.end() );

            assert( l_thingToErase.value() == (zmQTreeWidgetItem*)item );
            assert( l_itemToErase.value() == uid );

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
            assert( l_it != m_thing_lwitem_map.end() );
            return l_it.value();
        }

        std::string get( QTreeWidgetItem *item ) const
        {
            QMap< zmQTreeWidgetItem *, std::string >::const_iterator l_it =
                    m_lwitem_thing_map.find( (zmQTreeWidgetItem*)item );
            assert( l_it != m_lwitem_thing_map.end() );
            return l_it.value();
        }

        const std::string & get( zmQTreeWidgetItem *item ) const
        {
            QMap< zmQTreeWidgetItem *, std::string >::const_iterator l_it =
                    m_lwitem_thing_map.find( item );
            assert( l_it != m_lwitem_thing_map.end() );
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

    } m_widget_item_mapper;


public:

    explicit MainWindow(QWidget *parent = 0)
        : QMainWindow       ( parent )
        , m_ui              ( new Ui_window )
        , m_selected_thing  ( "" )
        , m_selected_twItem ( NULL )
    {   tracemessage( __FUNCTION__ );

        m_ui->setupUi( this );

        importFromFs();

        m_liToday = new QTreeWidgetItem();
        m_liToday->setText( 0, "TODAY");
        m_ui->twTask->addTopLevelItem( m_liToday );
        m_liToday->setExpanded( true );
        // should be an independent object of course
        QBrush b = m_liToday->foreground(0);
        b.setColor( Qt::darkBlue );
        m_liToday->setForeground( 0, b );
        m_liToday->setBackgroundColor( 0, Qt::lightGray );

        m_liInbox = new QTreeWidgetItem();
        m_liInbox->setText( 0, "INBOX");
        m_ui->twTask->addTopLevelItem( m_liInbox );
        m_liInbox->setExpanded( true );
        m_liInbox->setForeground( 0, b );
        m_liInbox->setBackgroundColor( 0, Qt::lightGray );

        m_liProjects = new QTreeWidgetItem();
        m_liProjects->setText( 0, "PROJECTS");
        m_ui->twTask->addTopLevelItem( m_liProjects );
        m_liProjects->setExpanded( true );
        m_liProjects->setForeground( 0, b );
        m_liProjects->setBackgroundColor( 0, Qt::lightGray );

        m_liContexts = new QTreeWidgetItem();
        m_liContexts->setText( 0, "CONTEXT");
        m_ui->twTask->addTopLevelItem( m_liContexts );
        m_liContexts->setExpanded( true );
        m_liContexts->setForeground( 0, b );
        m_liContexts->setBackgroundColor( 0, Qt::lightGray );

        m_liDone = new QTreeWidgetItem();
        m_liDone->setText( 0, "DONE");
        m_ui->twTask->addTopLevelItem( m_liDone );
        m_liDone->setExpanded( false );
        m_liDone->setForeground( 0, b );
        m_liDone->setBackgroundColor( 0, Qt::lightGray );

        updateUi();
    }

    virtual ~MainWindow()
    {
        delete m_ui;
    }

private:

    void addListItem( const std::string uid )
    {
        std::time_t l_creationTime = m_model.getCreationTime( uid );

        zmQTreeWidgetItem *l_item = new zmQTreeWidgetItem( l_creationTime );

        l_item->setText( 0, m_model.getCaption( uid ) );
        l_item->setText( 1, QString().sprintf("%ld",l_creationTime) );

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

        else if( m_model.isProjectItem( uid ) )
        {
            m_liProjects->addChild( l_item );
            m_liProjects->setText( 0, QString().sprintf("PROJECTS (%d)", m_liProjects->childCount()) );
        }

        else if( m_model.isTaskItem( uid ) )
        {
            std::string l_parentProject = m_model.getParentProject( uid );

            // todo: make sure l_project exists
            zmQTreeWidgetItem *l_project = m_widget_item_mapper.get(l_parentProject);

            l_project->addChild( l_item );
        }

        m_widget_item_mapper.add( uid, l_item );
    }

    void updateUi()
    {
        /// enforce list filling order by now..

        BOOST_FOREACH( const std::string& p, m_model.getProjectItems() )
        {
            addListItem( p );
        }

        BOOST_FOREACH( const std::string& t, m_model.getTaskItems() )
        {
            addListItem( t );
        }

        BOOST_FOREACH( const std::string& i, m_model.getInboxItems() )
        {
            addListItem( i );
        }
    }

    void importFromFs()
    {   tracemessage( __FUNCTION__ );

        m_model.load();
    }

    void updateGui()
    {   tracemessage( __FUNCTION__ );

    }

    void closeEvent( QCloseEvent *event )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        //exportToFs();
    }

private slots:
    void createInboxItemFromUiElements()
    {
        std::string l_item_uid = m_model.createNewInboxItem( m_ui->leCommand->text() );

        m_model.setNote( l_item_uid, m_ui->teNotes->toPlainText() );

        addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }

    void on_pbAddInboxItem_clicked()
    {   //tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        createInboxItemFromUiElements();
    }

    void on_leCommand_returnPressed()
    {   //tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        createInboxItemFromUiElements();
    }

    void on_twTask_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous )
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

            m_selected_twItem = current;
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

    void on_twTask_itemChanged( QTreeWidgetItem *item )
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

    void on_twTask_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target )
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
         && m_model.isProjectItem( l_target ) )
        {
            m_model.registerItemAsTask( l_source, l_target );

            // NOTE: this is black magic - don't touch! why does m_selected_twItem
            //       get set to NULL on removeChild()?!
            QTreeWidgetItem *l_uselessCopy( item );
            QTreeWidgetItem *l_groupingItem( item->parent() );
            l_groupingItem->removeChild( l_uselessCopy );
            target->addChild( l_uselessCopy );

        }
    }

    void on_pbClose_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
        if( m_selected_thing == "" ) return;

        tracemessage( "set item to DONE: %s (%s)",
                      m_selected_thing.c_str(),
                      m_model.getCaption( m_selected_thing ).toAscii().constData()  );

        m_model.setDone( m_selected_thing );

        // NOTE: this is black magic - don't touch! why does m_selected_twItem
        //       get set to NULL on removeChild()?!
        QTreeWidgetItem *l_uselessCopy( m_selected_twItem );
        QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
        l_groupingItem->removeChild( l_uselessCopy );
        m_liDone->addChild( l_uselessCopy );

        m_selected_thing = "";
        m_selected_twItem = NULL;
    }

    void on_pbDelete_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
        if( m_selected_thing == "" ) return;

        tracemessage( "erase item: %s (%s)",
                      m_selected_thing.c_str(),
                      m_model.getCaption( m_selected_thing ).toAscii().constData()  );

        m_model.eraseItem( m_selected_thing );
        //m_thing_lwitem_map.erase( m_thing_lwitem_map.find( m_selected_thing ) );
        m_selected_thing = "";

        //m_lwitem_thing_map.erase( m_lwitem_thing_map.find( m_selected_twItem ));

        m_widget_item_mapper.erase( m_selected_thing, m_selected_twItem );

        // dont alter m_selected_thing or m_selected_twItem after this
        // deletion since they get set there synchronously
        delete m_selected_twItem;
    }

    void on_pbMakeProject_clicked()
    {   tracemessage( __FUNCTION__ );

        QString l_project_name = m_ui->leCommand->text();

        /// if no name for a new item has been given cast an inbox item
        if( l_project_name == "" )
        {
            if( m_selected_thing == "" )
            {
                /// error
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
                QTreeWidgetItem *l_uselessCopy( m_selected_twItem );
                QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
                l_groupingItem->removeChild( l_uselessCopy );
                m_liProjects->addChild( l_uselessCopy );

                m_selected_thing = "";
                m_selected_twItem = NULL;
            }
            else
            {
                /// error
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
    }

    void on_pbMakeAction_clicked()
    {   tracemessage( __FUNCTION__ );

        QString l_project_name = m_ui->leCommand->text();

        /// if no name for a new item has been given cast an inbox item
        if( l_project_name == "" )
        {
            if( m_selected_thing == "" )
            {
                /// error
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
                QTreeWidgetItem *l_uselessCopy( m_selected_twItem );
                QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
                l_groupingItem->removeChild( l_uselessCopy );
                m_liProjects->addChild( l_uselessCopy );

                m_selected_thing = "";
                m_selected_twItem = NULL;
            }
            else
            {
                /// error
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
    }

    void on_pbOrder_clicked()
    {
        std::string l_taskUid = m_model.orderATask();

        if( l_taskUid == "" )
        {
            return;
        }

        QString l_taskCaption = m_model.getCaption( l_taskUid );
        QMessageBox::information( this, "just do it!", l_taskCaption );
    }

    void on_leCommand_textChanged ( const QString & text )
    {   //tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt
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
        m_selected_thing = "";
        m_selected_twItem = NULL;
        m_ui->twTask->unselect();
    }

#if 0
    void on_twTask_itemActivated( QTreeWidgetItem *item, int )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
        //m_ui->twTask->currentItemChanged();
    }

    void on_twTask_clicked( const QModelIndex &index )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
    }

    void on_twTask_doubleClicked( const QModelIndex &index )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;

        //m_ui->twTask->edit(index);
    }

    void on_twTask_itemDoubleClicked( QTreeWidgetItem *item, int column )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;

    }

    void update()
    {   tracemessage( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //tracemessage( "%d",m_model.getCurrentTaskDuration() );
    }


    void on_twTask_itemSelectionChanged ()
    {   tracemessage( __FUNCTION__ );
    }

    void on_leCommand_cursorPositionChanged ( int, int )
    {   tracemessage( __FUNCTION__ );
    }

    void on_leCommand_editingFinished ()
    {
        tracemessage( __FUNCTION__ );
    }

    void on_leCommand_selectionChanged ()
    {
        tracemessage( __FUNCTION__ );
    }

    void on_leCommand_textEdited ( const QString & text )
    {
        tracemessage( __FUNCTION__ );
    }
#endif

};

#endif // MAINWINDOW_H

