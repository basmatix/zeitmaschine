/// -*- coding: utf-8 -*-
///
/// file: zmQtUi.h
///
/// Copyright (C) 2013 Frans Fuerst
///


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_zeitmaschine.h"

#include "zmQtGtdModel.h"
#include "zmOsal.h"

#include "zmTrace.h"

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

#include <iostream>
#include <assert.h>

class QListWidgetItem;

class zmQtUi
        : public QMainWindow
{
    Q_OBJECT

    zmQtUi( const zmQtUi & );
    zmQtUi & operator= ( const zmQtUi & );

private:

    Ui_window          *m_ui;

    zmQtGtdModel        m_model;

    std::string         m_selected_thing;
    zmQTreeWidgetItem  *m_selected_twItem;

    QTreeWidgetItem    *m_liToday;
    QTreeWidgetItem    *m_liInbox;
    QTreeWidgetItem    *m_liProjects;
    QTreeWidgetItem    *m_liContexts;
    QTreeWidgetItem    *m_liDone;
    MySortFilterProxyModel *m_searchProxy;

    class WidgetItemMapper
    {
        QMap< std::string, zmQTreeWidgetItem * > m_thing_lwitem_map;
        QMap< zmQTreeWidgetItem *, std::string > m_lwitem_thing_map;

    public:

        WidgetItemMapper()
            : m_thing_lwitem_map()
            , m_lwitem_thing_map()
        {

        }

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

    explicit zmQtUi(QWidget *parent = 0);

    virtual ~zmQtUi();

private:

    void addListItem( const std::string uid );

    void updateUi();

    void updateGui()
    {   tracemessage( __FUNCTION__ );

    }

    void closeEvent( QCloseEvent *event );

    void unselect();

private slots:

    void createInboxItemFromUiElements();

    void on_pbAddInboxItem_clicked();

    void on_leCommand_returnPressed();

    void on_twMaster_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous );

    void on_twMaster_itemChanged( QTreeWidgetItem *item );

    void on_twMaster_itemDropped( QTreeWidgetItem *item, QTreeWidgetItem *target );

    void on_pbClose_clicked();

    void on_pbDelete_clicked();

    void on_pbPlusOne_clicked();

    void on_pbMakeProject_clicked();

    void on_pbMakeNextAction_clicked();

    void on_pbMakeAction_clicked();

    void on_pbOrder_clicked();

    void on_leCommand_textChanged ( const QString & text );

#if 0
    void on_twMaster_itemActivated( QTreeWidgetItem *item, int )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
        //m_ui->twMaster->currentItemChanged();
    }

    void on_twMaster_clicked( const QModelIndex &index )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;
    }

    void on_twMaster_doubleClicked( const QModelIndex &index )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;

        //m_ui->twMaster->edit(index);
    }

    void on_twMaster_itemDoubleClicked( QTreeWidgetItem *item, int column )
    {   tracemessage( __FUNCTION__ );
        //std::cout << index. << std::endl;;

    }

    void update()
    {   tracemessage( __FUNCTION__ );
        //m_ui->lblTask->setText( QString().sprintf("%d",m_model.getCurrentTaskDuration()) );
        //tracemessage( "%d",m_model.getCurrentTaskDuration() );
    }

    void on_twMaster_itemSelectionChanged ()
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

