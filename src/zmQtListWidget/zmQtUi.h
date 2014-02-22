/// -*- coding: utf-8 -*-
///
/// file: zmQtUi.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_zmQtListWidget.h"

#include "zmQtGtdModel.h"

class QListWidgetItem;

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

    MainWindow( const MainWindow & );
    MainWindow & operator= ( const MainWindow & );

private:

    Ui_window          *m_ui;

    zmQtGtdModel        m_model;

    std::string         m_selected_thing;
    zmQTreeWidgetItem  *m_selected_twItem;

public:

    explicit MainWindow(QWidget *parent = 0);

    virtual ~MainWindow();

private:

    void updateGui();

    void closeEvent( QCloseEvent *event );

    void unselect();

private slots:

    void createInboxItemFromUiElements();

    void on_pbAddInboxItem_clicked();

    void on_leCommand_returnPressed();

    void on_twTask_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous );

    void on_twTask_itemChanged( QTreeWidgetItem *item );

    void on_twTask_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target );

    void on_pbClose_clicked();

    void on_pbDelete_clicked();

    void on_pbPlusOne_clicked();

    void on_pbMakeProject_clicked();

    void on_pbMakeNextAction_clicked();

    void on_pbMakeAction_clicked();

    void on_pbOrder_clicked();

    void on_pbSync_clicked();

    void on_pbSnapshot_clicked();

    void on_leCommand_textChanged ( const QString & text );

#if 0
    void on_twTask_itemActivated( QTreeWidgetItem *item, int )
    {   trace_i( __FUNCTION__ );
        //std::cout << index. << std::endl;;
        //m_ui->twTask->currentItemChanged();
    }

    void on_twTask_clicked( const QModelIndex &index )
    {   trace_i( __FUNCTION__ );
        //std::cout << index. << std::endl;;
    }

    void on_twTask_doubleClicked( const QModelIndex &index )
    {   trace_i( __FUNCTION__ );
        //std::cout << index. << std::endl;;

        //m_ui->twTask->edit(index);
    }

    void on_twTask_itemDoubleClicked( QTreeWidgetItem *item, int column )
    {   trace_i( __FUNCTION__ );
        //std::cout << index. << std::endl;;

    }

    void update()
    {   trace_i( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //trace_i( "%d",m_model.getCurrentTaskDuration() );
    }

    void on_twTask_itemSelectionChanged ()
    {   trace_i( __FUNCTION__ );
    }

    void on_leCommand_cursorPositionChanged ( int, int )
    {   trace_i( __FUNCTION__ );
    }

    void on_leCommand_editingFinished ()
    {
        trace_i( __FUNCTION__ );
    }

    void on_leCommand_selectionChanged ()
    {
        trace_i( __FUNCTION__ );
    }

    void on_leCommand_textEdited ( const QString & text )
    {
        trace_i( __FUNCTION__ );
    }

#endif

};

#endif // MAINWINDOW_H

