/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_zeitmaschine.h"

#include "zmGtdModel.h"

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

    zmGtdModel       m_model;
    std::string      m_filename;

    std::string      m_selected_thing;
    QTreeWidgetItem *m_selected_twItem;

    QTreeWidgetItem *m_liToday;
    QTreeWidgetItem *m_liInbox;
    QTreeWidgetItem *m_liProjects;
    QTreeWidgetItem *m_liContexts;
    QTreeWidgetItem *m_liDone;

    QMap< QTreeWidgetItem *, std::string > m_lwitem_thing_map;
    QMap< std::string, QTreeWidgetItem * > m_thing_lwitem_map;

public:

    explicit MainWindow(QWidget *parent = 0)
        : QMainWindow       ( parent )
        , m_ui              ( new Ui_window )
        , m_filename        ( "zeitmaschine/export.yaml" )
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
        QTreeWidgetItem *l_item = new QTreeWidgetItem();

        QString l_qcaption = QString::fromUtf8(
                    m_model.getCaption( uid ).c_str() );

        l_item->setText( 0, l_qcaption );

        tracemessage( "adding item %s to list (%s)",
                      uid.c_str(),
                      m_model.getCaption( uid ).c_str()  );

        if( m_model.isDone( uid ) )
        {
            m_liDone->addChild( l_item );
        }

        else if( m_model.isInboxItem( uid ) )
        {
            m_liInbox->addChild( l_item );
        }

        else if( m_model.isProjectItem( uid ) )
        {
            m_liProjects->addChild( l_item );
        }

        else if( m_model.isTaskItem( uid ) )
        {
            std::string l_parentProject = m_model.getParentProject( uid );

            QTreeWidgetItem *l_project = m_thing_lwitem_map[l_parentProject];

            l_project->addChild( l_item );
        }

        l_item->setFlags( l_item->flags() | Qt::ItemIsEditable );

        m_lwitem_thing_map[ l_item ] = uid;
        m_thing_lwitem_map[ uid ] = l_item;
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

    void exportToFs()
    {   tracemessage( __FUNCTION__ );

        m_model.save( m_filename );
    }

    void importFromFs()
    {   tracemessage( __FUNCTION__ );

        m_model.load( m_filename );
    }

    void updateGui()
    {   tracemessage( __FUNCTION__ );

    }

    void closeEvent( QCloseEvent *event )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        exportToFs();
    }

private slots:

    void on_leCommand_returnPressed ()
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        std::string l_caption( m_ui->leCommand->text().toUtf8().constData() );

        std::string l_note( m_ui->teNotes->toPlainText().toUtf8().constData() );

        std::string l_item_uid = m_model.createNewInboxItem( l_caption );

        m_model.setNote( l_item_uid, l_note );

        addListItem( l_item_uid );

        m_ui->leCommand->setText("");

        exportToFs();
    }

    void on_twTask_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous )
    {   //tracemessage( __FUNCTION__ );

        if( m_lwitem_thing_map.contains( previous ) )
        {
            /// if there has been selected - save it's note
            std::string l_previous_thing = m_lwitem_thing_map[ previous ];

            //assert( l_previous_thing == m_selected_thing );
            if( previous == m_selected_twItem )
            {
                m_model.setNote( l_previous_thing, m_ui->teNotes->toPlainText().toUtf8().constData() );
            }
        }

        m_ui->teNotes->setText( "" );

        if( m_lwitem_thing_map.contains( current ) )
        {

            m_selected_twItem = current;
            m_selected_thing = m_lwitem_thing_map[ m_selected_twItem ];
            tracemessage( "clicked on item %s (%s)",
                          m_selected_thing.c_str(),
                          m_model.getCaption( m_selected_thing ).c_str()  );
        }
        else
        {
            m_selected_thing = "";
            m_selected_twItem = NULL;
        }

        m_ui->teNotes->setText( QString::fromUtf8(
                    m_model.getNote( m_selected_thing ).c_str() ) );
    }

    void on_twTask_itemChanged( QTreeWidgetItem *item )
    {   //tracemessage( __FUNCTION__ );

        if( m_lwitem_thing_map.contains( item ) )
        {
            std::string l_thing = m_lwitem_thing_map[ item ];
            std::string l_new_caption = item->text(0).toUtf8().constData();
            tracemessage( "changing item text from '%s' to '%s'",
                          m_model.getCaption( l_thing ).c_str(),
                          l_new_caption.c_str() );
            m_model.setCaption( l_thing, l_new_caption );

            exportToFs();
        }
    }

    void on_twTask_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target )
    {   //tracemessage( __FUNCTION__ );

        if( !m_lwitem_thing_map.contains( item ) )
        {
            return;
        }
        if( !m_lwitem_thing_map.contains( target ) )
        {
            return;
        }

        std::string l_source = m_lwitem_thing_map[ item ];
        std::string l_target = m_lwitem_thing_map[ target ];

        tracemessage( "dragged '%s' to '%s'",
                      m_model.getCaption( l_source ).c_str(),
                      m_model.getCaption( l_target ).c_str() );

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
                      m_model.getCaption( m_selected_thing ).c_str()  );

        m_model.setDone( m_selected_thing );

        // NOTE: this is black magic - don't touch! why does m_selected_twItem
        //       get set to NULL on removeChild()?!
        QTreeWidgetItem *l_uselessCopy( m_selected_twItem );
        QTreeWidgetItem *l_groupingItem( m_selected_twItem->parent() );
        l_groupingItem->removeChild( l_uselessCopy );
        m_liDone->addChild( l_uselessCopy );

        m_selected_thing = "";
        m_selected_twItem = NULL;

        exportToFs();
    }

    void on_pbDelete_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
        if( m_selected_thing == "" ) return;

        tracemessage( "erase item: %s (%s)",
                      m_selected_thing.c_str(),
                      m_model.getCaption( m_selected_thing ).c_str()  );

        m_model.eraseItem( m_selected_thing );
        m_thing_lwitem_map.erase( m_thing_lwitem_map.find( m_selected_thing ) );
        m_selected_thing = "";

        m_lwitem_thing_map.erase( m_lwitem_thing_map.find( m_selected_twItem ));

        // dont alter m_selected_thing or m_selected_twItem after this
        // deletion since they get set there synchronously
        delete m_selected_twItem;

        exportToFs();
    }

    void on_pbMakeProject_clicked()
    {   tracemessage( __FUNCTION__ );
        std::string l_project_name = m_ui->leCommand->text().trimmed().toUtf8().constData();

        if( l_project_name == "" )
        {

        }
        else
        {
            tracemessage( "create project '%s' from magic line",
                          l_project_name.c_str() );

            std::string l_item_uid = m_model.createProject( l_project_name );

            addListItem( l_item_uid );

            m_ui->leCommand->setText("");

            exportToFs();
        }
    }

    void on_pbOrder_clicked()
    {
        std::string l_taskUid = m_model.orderATask();

        if( l_taskUid == "" )
        {
            return;
        }

        QString l_taskCaption = QString::fromUtf8( m_model.getCaption( l_taskUid ).c_str() );
        QMessageBox::information( this, "just do it!", l_taskCaption );
    }

    void on_leCommand_textChanged ( const QString & text )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt
        std::string l_search_string = text.toLower().toUtf8().constData();
        for( QMap< std::string, QTreeWidgetItem * >::iterator
             i  = m_thing_lwitem_map.begin();
             i != m_thing_lwitem_map.end(); ++i )
        {
            bool l_search_string_matches = m_model.itemContentMatchesString(
                        i.key(), l_search_string );
            i.value()->setHidden( ! l_search_string_matches );
        }

        /// eventually current note save
        if( m_selected_thing != "" )
        {
            m_model.setNote( m_selected_thing, m_ui->teNotes->toPlainText().toUtf8().constData() );
            m_ui->teNotes->setText( "" );
            assert( m_selected_twItem == m_thing_lwitem_map[m_selected_thing] );
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

