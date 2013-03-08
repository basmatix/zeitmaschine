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
    Model       m_model;

public:

    explicit MainWindow(QWidget *parent = 0)
        : QMainWindow   ( parent )
        , m_ui          ( new Ui_window )
    {   tracemessage( __FUNCTION__ );

        m_ui->setupUi( this );

    }

    virtual ~MainWindow()
    {
        delete m_ui;
    }

private:

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

        m_model.save( "flow.yaml" );
    }

    void dump()
    {
    }

private slots:

    void on_leCommand_returnPressed ()
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

        m_model.createNewItem( m_ui->leCommand->text().toStdString() );
        m_ui->lwTask->addItem( m_ui->leCommand->text() );
        m_ui->leCommand->setText("");
    }

    void update()
    {   tracemessage( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //tracemessage( "%d",m_model.getCurrentTaskDuration() );
    }

    void on_lwTask_currentTextChanged ( const QString & currentText )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

    }

    void on_leCommand_textChanged ( const QString & text )
    {   tracemessage( __FUNCTION__ );
        // this method is being called automatically by Qt

    }

    void on_pbAdd_clicked()
    {   tracemessage( __FUNCTION__ );

    }

    void on_pbClose_clicked()
    {   tracemessage( __FUNCTION__ );

        //QMutexLocker monitor( &m_mutex );
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

