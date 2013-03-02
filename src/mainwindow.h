#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
(L) Requirements for Flow
    [ ] having an Inbox
    [ ] having contexts
    [ ] having sub contexts which can be subcontext elsewhere too
    [ ] contexts can have files opened
    [ ] files can get synchronized to wikis
    [ ] switching tasks etc gets logged

    [ ] data being locally encrypted
    [ ] jounaled file based -> conflict safe

    [ ] Inbox items can be added without authorization (eg by others)
    [ ] service based
*/

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
#include <assert.h>

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
    QMutex      m_mutex;
    Model       m_model;

public:

    explicit MainWindow(QWidget *parent = 0)
        : QMainWindow   ( parent )
        , m_ui          ( new Ui_window )
    {   tracemessage( __FUNCTION__ );

        m_ui->setupUi( this );

        //m_ui->teNotes->setEnabled( false );

        importFromFs();

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(1000);
    }

    virtual ~MainWindow()
    {
        delete m_ui;
    }

private:

    void switchToTask( const QString &taskName )
    {   tracemessage( __FUNCTION__ );

        QMutexLocker monitor( &m_mutex );


//        m_ui->teNotes->setPlainText( m_model.getCurrentTaskNotes() );
//        m_ui->teNotes->setEnabled( m_model.hasCurrentTask() );
    }

    void exportToFs()
    {   tracemessage( __FUNCTION__ );

        QMutexLocker monitor( &m_mutex );

        QFile l_file( "file.dat" );
        l_file.open( QIODevice::WriteOnly );
        QDataStream l_dsOut( &l_file );
        //l_dsOut << m_model.m_texts;

        tracemessage("export %d values:", m_model.m_texts.size() );
        /*
        QMapIterator< QString, Task > i( m_model.m_texts );
        while( i.hasNext() )
        {
            i.next();
            tracemessage("    %s(%s) = '%s'",
                         i.key().toAscii().data(),
                         i.value().m_active?"active":"closed",
                         i.value().m_text.toAscii().data() );
            m_ui->lwTask->addItem( i.key() );
        }
        */
    }

    void importFromFs()
    {   tracemessage( __FUNCTION__ );

        QMutexLocker monitor( &m_mutex );

        QFile l_file("file.dat");
        l_file.open( QIODevice::ReadOnly );
        QDataStream l_dsIn( &l_file );
        //l_dsIn >> m_model.m_texts;
        /*
        tracemessage("imported %d items:", m_model.m_texts.size() );

        QMapIterator< QString, Task > i( m_model.m_texts );
        while( i.hasNext() )
        {
            i.next();
            tracemessage("    %s(%s) = '%s'",
                         i.key().toAscii().data(),
                         i.value().m_active?"active":"closed",
                         i.value().m_text.toAscii().data() );
        }
        */
        updateGui();
    }

    void updateGui()
    {   tracemessage( __FUNCTION__ );
/*
        m_ui->lwTask->clear();

        QMapIterator< QString, Task > i( m_model.m_texts );
        while( i.hasNext() )
        {
            i.next();
            if( i.value().m_active )
            {
                QListWidgetItem *l_item = new QListWidgetItem( i.key() );
                //l_item->setTextColor(QColor("Grey"));
                m_ui->lwTask->addItem( l_item );
            }
        }
        */
    }

    void closeEvent( QCloseEvent *event )
    {   tracemessage( __FUNCTION__ );

        //m_model.setCurrentNote( m_ui->teNotes->toPlainText() );

        exportToFs();
    }

    void dump()
    {
        /*
        tracemessage("currently model contains %d items:", m_model.m_texts.size() );
        QMapIterator< QString, Task > i( m_model.m_texts );
        while( i.hasNext() )
        {
            i.next();
            tracemessage("    %s(%s) = '%s'",
                         i.key().toAscii().data(),
                         i.value().m_active?"active":"closed",
                         i.value().m_text.toAscii().data() );
        }
        */
    }

private slots:

    void update()
    {   //tracemessage( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //tracemessage( "%d",m_model.getCurrentTaskDuration() );
    }

    void on_lwTask_currentTextChanged ( const QString & currentText )
    {   tracemessage( __FUNCTION__ );

        tracemessage( "%s",currentText.toAscii().data() );

        switchToTask( currentText );
    }

    void on_leCommand_textChanged ( const QString & text )
    {   tracemessage( __FUNCTION__ );
    }

    void on_pbAdd_clicked()
    {   tracemessage( __FUNCTION__ );

        //m_ui->lwTask->addItem( m_ui->leCommand->text() );
        //m_ui->leCommand->clear();
    }

    void on_pbClose_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
        /*
        tracemessage( "close task '%s'", m_model.m_currentTask.toAscii().data() );

        QMap< QString, Task >::iterator i( m_model.m_texts.find( m_model.m_currentTask ) );
        if( i == m_model.m_texts.end() )
        {
            tracemessage( "tried to close nonexistent task" );
            return;
        }

        (*i).m_active = false;
        assert( m_ui->lwTask->currentItem()->text() == m_model.m_currentTask );
        QListWidgetItem *l_item = m_ui->lwTask->currentItem();
        assert( l_item != NULL );
        */
        // will handle deletion from list and synchronously make the
        // list widget update - so this is a potential deadlock source!
        //delete l_item;
    }

    void on_leCommand_returnPressed ()
    {   tracemessage( __FUNCTION__ );
    }

#if 1
    void on_lwTask_clicked ( const QModelIndex & index )
    {    tracemessage( __FUNCTION__ );
    }

    void on_lwTask_currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_currentRowChanged ( int currentRow )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemActivated ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemChanged ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemClicked ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemDoubleClicked ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
        //m_model.
    }

    void on_lwTask_itemEntered ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemPressed ( QListWidgetItem * item )
    {
        tracemessage( __FUNCTION__ );
    }

    void on_lwTask_itemSelectionChanged ()
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

