#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_flow2.h"

#include "FlowModel.h"

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

#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>

class QListWidgetItem;


inline void tracemessage( const char * a_format, ... )
{
    char l_buffer[1024];
    va_list l_args;
    va_start (l_args, a_format);
    vsprintf ( l_buffer, a_format, l_args );
    va_end( l_args );
    printf( "%s\n", l_buffer );
    fflush( stdout );
}

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

private:

    Ui_window  *m_ui;
    Model       m_model;
    std::string m_selected_item;

    QTreeWidgetItem *m_liToday;
    QTreeWidgetItem *m_liInbox;
    QTreeWidgetItem *m_liProjects;
    QTreeWidgetItem *m_liContexts;

    //QMap< std::string, QTreeWidgetItem *> m_thing_item_map;
    QMap< QTreeWidgetItem *, std::string > m_item_thing_map;
    std::string m_filename;

public:

    explicit MainWindow(QWidget *parent = 0)
        : QMainWindow    ( parent )
        , m_ui           ( new Ui_window )
        , m_filename     ( ".flow2/export.yaml" )
        , m_selected_item( "" )
    {   tracemessage( __FUNCTION__ );

        m_ui->setupUi( this );

        m_model.load( m_filename );

        m_liToday = new QTreeWidgetItem();
        m_liToday->setText( 0, "today");
        m_ui->twTask->addTopLevelItem( m_liToday );

        m_liInbox = new QTreeWidgetItem();
        m_liInbox->setText( 0, "inbox");
        m_ui->twTask->addTopLevelItem( m_liInbox );

        m_liProjects = new QTreeWidgetItem();
        m_liProjects->setText( 0, "projects");
        m_ui->twTask->addTopLevelItem( m_liProjects );

        m_liProjects = new QTreeWidgetItem();
        m_liProjects->setText( 0, "context");
        m_ui->twTask->addTopLevelItem( m_liProjects );

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
        l_item->setText( 0, QString::fromStdString( m_model.getCaption( uid ) ));
        if( m_model.hasAttribute( uid, "gtd_item_unhandled" ) )
        {
            m_liInbox->addChild( l_item );
        }

        //m_thing_item_map[uid] = l_item;
        m_item_thing_map[ l_item ] = uid;
    }

    void updateUi()
    {
        BOOST_FOREACH(const Model::FlowModelMapType::value_type& i, m_model.things() )
        {
            addListItem( i.first );

            tracemessage( "setup '%s' caption = '%s'", i.first.c_str(), i.second->m_caption.c_str() );
        }
    }

    void exportToFs()
    {   tracemessage( __FUNCTION__ );

    }

    void importFromFs()
    {   tracemessage( __FUNCTION__ );

    }

    void updateGui()
    {   tracemessage( __FUNCTION__ );

    }

    void closeEvent( QCloseEvent *event )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        m_model.save( m_filename );
    }


private slots:

    void on_leCommand_returnPressed ()
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        std::string l_item_uid = m_model.createNewItem( m_ui->leCommand->text().toStdString() );

        m_model.addAttribute( l_item_uid, "gtd_item_unhandled" );

        addListItem( l_item_uid );

        m_ui->leCommand->setText("");
    }

    void on_twTask_itemActivated( QTreeWidgetItem *item, int )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
        //m_ui->twTask->currentItemChanged();
    }

    void on_twTask_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous )
    {   tracemessage( __FUNCTION__ );

        if( m_item_thing_map.contains( current ) )
        {
            m_selected_item = m_item_thing_map[ current ];
            tracemessage( "clicked on item %s (%s)",
                          m_selected_item.c_str(),
                          m_model.getCaption( m_selected_item ).c_str()  );

        }
        else
        {
            m_selected_item = "";
        }
    }

    void on_twTask_clicked( const QModelIndex &index )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
    }

    void on_pbDelete_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
    }

    void update()
    {   tracemessage( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //tracemessage( "%d",m_model.getCurrentTaskDuration() );
    }

    void on_leCommand_textChanged ( const QString & text )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

    }


#if 1


    void on_twTask_itemActivated ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_twTask_itemChanged ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_twTask_itemClicked ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_twTask_itemDoubleClicked ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
        //m_model.
    }

    void on_twTask_itemEntered ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_twTask_itemPressed ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_twTask_itemSelectionChanged ()
    {
        tracemessage( __FUNCTION__ );
    }

    void on_leCommand_cursorPositionChanged ( int , int )
    {
        tracemessage( __FUNCTION__ );
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

