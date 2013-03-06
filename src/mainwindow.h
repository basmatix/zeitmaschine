#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_brainstack.h"

#include "codeeditor.h"

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

// [X] close items
// [ ] track tasks - recognize begin, end
// [ ] add/close plausibility checks
// [ ] backup saving
// [ ] link items
// [ ] find on typing
// [ ] save in a readable way
// [ ] connect to ysbox
// [ ] be SparkleShare compatible
// [ ] have a standard set of open files (per task?)

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

class Task
{
public:

    Task()
        : m_text            ( "" )
        , m_active          ( true )
        , m_summarizedTime  ( 0 )
    {
        /// don't add vital code here - this is just for
        /// some containers needing a standard constructor
        m_durationMeter.start();
    }

    Task( const QString &text )
        : m_text            ( text )
        , m_active          ( true )
        , m_summarizedTime  ( 0 )
    {
        m_durationMeter.start();
        m_durationMeter.restart();
        tracemessage( "%d", m_durationMeter.elapsed() );
    }

    void activate()
    {
        m_activeTimes.append(
                    QPair< QDateTime, QDateTime >(
                        QDateTime::currentDateTime(),QDateTime()) );
        m_durationMeter.restart();
    }

    void deactivate()
    {
        assert( !m_activeTimes.empty() );
        m_activeTimes.back().second = QDateTime::currentDateTime();
        m_summarizedTime += m_durationMeter.elapsed();
    }

    void close()
    {
        deactivate();
    }

    int getDuration() const
    {
        // tracemessage( "%d", m_durationMeter.elapsed() );
        return m_durationMeter.elapsed();
    }

    bool m_active;
    QString m_text;
    int m_summarizedTime;
    QTime m_durationMeter;
    QList< QPair< QDateTime,QDateTime > > m_activeTimes;
};

class Model
{
public:
    void activate( const QString &taskName )
    {
        QMap< QString, Task >::iterator i( m_texts.find( taskName ) );

        /// does a task with the given name exist?
        if( m_texts.find( taskName ) == m_texts.end() )
        {
            i = m_texts.insert( taskName, Task("") );
        }

        i->activate();

        m_currentTask = taskName;
    }

    void deactivate();

    bool hasCurrentTask() const
    {
        return m_currentTask != "";
    }

    const QString & getCurrentTaskNotes() const
    {
        assert( hasCurrentTask() );

        assert( m_texts.find( m_currentTask ) != m_texts.end() );

        return m_texts.find( m_currentTask )->m_text;
    }

    int getCurrentTaskDuration()
    {
        if( !hasCurrentTask() ) return 0;

        assert( m_texts.find( m_currentTask ) != m_texts.end() );

        return m_texts.find( m_currentTask )->getDuration();
    }

    void setCurrentNote( const QString &text )
    {
        ///.current task must be set
        assert( m_currentTask != "" || text == "" );

        if( m_currentTask == "" ) return;

        assert( m_texts.find( m_currentTask ) != m_texts.end() );

        m_texts[ m_currentTask ].m_text = text;
    }

    QMap< QString, Task >   m_texts;
    QString                 m_currentTask;
};

/// serialization
inline QDataStream & operator<<( QDataStream &out, const Task &task )
{
    out << task.m_active << task.m_text;
    return out;
}

/// deserialization
inline QDataStream & operator>>( QDataStream &in, Task &task )
{
    in >> task.m_active >> task.m_text;
    return in;
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

        m_ui->teNotes->setEnabled( false );

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

        /// before we change the task - write back all changes
        m_model.setCurrentNote( m_ui->teNotes->toPlainText() );

        m_model.activate( taskName );

        m_ui->teNotes->setPlainText( m_model.getCurrentTaskNotes() );
        m_ui->teNotes->setEnabled( m_model.hasCurrentTask() );
    }

    void exportToFs()
    {   tracemessage( __FUNCTION__ );

        QMutexLocker monitor( &m_mutex );

        QFile l_file( "file.dat" );
        l_file.open( QIODevice::WriteOnly );
        QDataStream l_dsOut( &l_file );
        l_dsOut << m_model.m_texts;

        tracemessage("export %d values:", m_model.m_texts.size() );
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
    }

    void importFromFs()
    {   tracemessage( __FUNCTION__ );

        QMutexLocker monitor( &m_mutex );

        QFile l_file("file.dat");
        l_file.open( QIODevice::ReadOnly );
        QDataStream l_dsIn( &l_file );
        l_dsIn >> m_model.m_texts;

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
        updateGui();
    }

    void updateGui()
    {   tracemessage( __FUNCTION__ );

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
    }

    void closeEvent( QCloseEvent *event )
    {   tracemessage( __FUNCTION__ );

        m_model.setCurrentNote( m_ui->teNotes->toPlainText() );

        exportToFs();
    }

    void dump()
    {
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
    }

private slots:

    void update()
    {   //tracemessage( __FUNCTION__ );
        m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
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

        m_ui->lwTask->addItem( m_ui->leCommand->text() );
        m_ui->leCommand->clear();
    }

    void on_pbClose_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
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

        // will handle deletion from list and synchronously make the
        // list widget update - so this is a potential deadlock source!
        delete l_item;
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

