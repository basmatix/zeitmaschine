/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMQTGTDADAPTER_H
#define ZMQTGTDADAPTER_H

#include <zmQtGtdItem.h>

#include <zm/zmGtdModel.h>
#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QSortFilterProxyModel>

class QTreeView;

class MySortFilterProxyModel
        : public QSortFilterProxyModel
{
    QString m_search;

public:

    MySortFilterProxyModel(QObject *parent = 0)
        : QSortFilterProxyModel( parent )
        , m_search( "" )
    {

    }

    void setSearchString( const QString &searchString )
    {
        m_search = searchString.toLower();
        invalidateFilter();
    }

private:

    bool filterAcceptsRow( int sourceRow,const QModelIndex &sourceParent ) const
    {
        if( m_search == "" ) return true;

        QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
        return (sourceModel()->data(index0).toString().toLower().contains( m_search ) );
    }

    /// we don't want sorting
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        return;
    }

};

/// std-c++/Qt - interface to zmGtdModel. Aims to be a
/// fully qualified Qt model in the future
class zmQtGtdModel
    : public QAbstractItemModel
{
    zmGtdModel      m_gtd_model;
    zmQtGtdItem    *m_rootItem;

    //zmQtGtdItem    *m_liToday;
    zmQtGtdItem    *m_liInbox;
    zmQtGtdItem    *m_liProjects;
    zmQtGtdItem    *m_liKnowledge;
    zmQtGtdItem    *m_liDone;

    zmQtGtdModel( const zmQtGtdModel & );
    zmQtGtdModel & operator= ( const zmQtGtdModel & );


    class WidgetItemMapper
    {
        QMap< std::string, zmQtGtdItem * > m_thing_lwitem_map;
        QMap< zmQtGtdItem *, std::string > m_lwitem_thing_map;

    public:

        WidgetItemMapper()
            : m_thing_lwitem_map()
            , m_lwitem_thing_map()
        {

        }

        void add( const std::string uid, zmQtGtdItem *item )
        {
            m_lwitem_thing_map[ item ] = uid;
            m_thing_lwitem_map[ uid ] = item;
        }

        void erase( const std::string uid, zmQtGtdItem *item )
        {
            QMap< std::string, zmQtGtdItem * >::iterator l_thingToErase =
                    m_thing_lwitem_map.find( uid );
            QMap< zmQtGtdItem *, std::string >::iterator l_itemToErase =
                    m_lwitem_thing_map.find( (zmQtGtdItem*)item );
            assert( l_thingToErase != m_thing_lwitem_map.end() );
            assert( l_itemToErase != m_lwitem_thing_map.end() );

            assert( l_thingToErase.value() == (zmQtGtdItem*)item );
            assert( l_itemToErase.value() == uid );

            m_thing_lwitem_map.erase( l_thingToErase );

            m_lwitem_thing_map.erase( l_itemToErase );
        }


        bool contains( zmQtGtdItem *item ) const
        {
            return m_lwitem_thing_map.find(item) != m_lwitem_thing_map.end();
        }

        zmQtGtdItem * get( const std::string uid )
        {
            QMap< std::string, zmQtGtdItem * >::iterator l_it =
                    m_thing_lwitem_map.find( uid );
            assert( l_it != m_thing_lwitem_map.end() );
            return l_it.value();
        }

        const std::string & get( zmQtGtdItem *item ) const
        {
            QMap< zmQtGtdItem *, std::string >::const_iterator l_it =
                    m_lwitem_thing_map.find( item );
            assert( l_it != m_lwitem_thing_map.end() );
            return l_it.value();
        }

        QMap< std::string, zmQtGtdItem * >::iterator begin()
        {
            return m_thing_lwitem_map.begin();
        }

        QMap< std::string, zmQtGtdItem * >::iterator end()
        {
            return m_thing_lwitem_map.end();
        }

    } m_widget_item_mapper;


/// maintenance interface
public:

    zmQtGtdModel();

    virtual ~zmQtGtdModel();

    void populate();

    void autoExpand( QTreeView * );

    void addListItem( const std::string uid );

    bool hasUsedUsername() const
    {
        return m_gtd_model.hasUsedUsername();
    }

    bool hasUsedHostname() const
    {
        return m_gtd_model.hasUsedHostname();
    }

    void setUsedUsername( const QString &username )
    {
        m_gtd_model.setUsedUsername( username.toUtf8().constData() );
    }

    void setUsedHostname( const QString &hostname )
    {
        m_gtd_model.setUsedHostname( hostname.toUtf8().constData() );
    }

    void setConfigPersistance( bool value )
    {
        m_gtd_model.setConfigPersistance( value );
    }

    void setLocalFolder( const QString &path )
    {
        m_gtd_model.setLocalFolder( path.toUtf8().constData() );
    }

    void addDomainSyncFolder( const QString &domainName, const QString &path )
    {
        m_gtd_model.addDomainSyncFolder( domainName.toUtf8().constData(), path.toUtf8().constData() );
    }

    void initialize()
    {
        m_gtd_model.initialize();

        populate();
    }

    void sync()
    {
        m_gtd_model.sync();
    }

    void merge( const QString &modelFile )
    {
        m_gtd_model.merge( modelFile.toUtf8().constData() );
    }

    // todo: eventually provide some sort of export functionality for
    //       development purposes
    // void save( const std::string &filename )
    //{
    //    m_gtd_model.save( filename );
    //}

///
/// QAbstractItemModel interface
///
    Qt::ItemFlags flags( const QModelIndex &index ) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex index( int row, int column, const QModelIndex &parent ) const;

    QModelIndex parent( const QModelIndex &index ) const;

    int rowCount( const QModelIndex &parent ) const;

    int columnCount( const QModelIndex &parent ) const;

    QVariant data( const QModelIndex &index, int role ) const;

/// const interface
public:

    QString getNote( const std::string &uid ) const
    {
        return QString::fromUtf8( m_gtd_model.getNote( uid ).c_str() );
    }

    std::time_t getCreationTime( const std::string &uid ) const
    {
        return m_gtd_model.getCreationTime( uid );
    }

    int getImportance( const std::string &uid ) const
    {
        return m_gtd_model.getImportance( uid );
    }

    std::list< std::string > getInboxItems( bool includeDoneItems ) const
    {
        return m_gtd_model.getInboxItems( includeDoneItems );
    }

    std::list< std::string > getTaskItems( bool includeStandaloneTasks, bool includeDoneItems ) const
    {
        return m_gtd_model.getTaskItems( includeStandaloneTasks, includeDoneItems );
    }

    std::list< std::string > getProjectItems( bool includeStandaloneTasks, bool includeDoneItems ) const
    {
        return m_gtd_model.getProjectItems( includeStandaloneTasks, includeDoneItems );
    }

    bool isTaskItem( const std::string &item, bool includeStandaloneTasks ) const
    {
        return m_gtd_model.isTaskItem( item, includeStandaloneTasks );
    }

    bool isInboxItem( const std::string &item ) const
    {
        return m_gtd_model.isInboxItem( item );
    }

    bool isProjectItem( const std::string &item, bool includeStandaloneTasks ) const
    {
        return m_gtd_model.isProjectItem( item, includeStandaloneTasks );
    }

    bool isDone( const std::string &task_item ) const
    {
        return m_gtd_model.isDone( task_item );
    }

    std::string getNextTask( const std::string &task_item ) const
    {
        return m_gtd_model.getNextTask( task_item );
    }

    std::string getParentProject( const std::string &task_item ) const
    {
        return m_gtd_model.getParentProject( task_item );
    }

    std::string orderATask() const
    {
        return m_gtd_model.orderATask();
    }

    const QString getCaption( const std::string &uid ) const
    {
        return QString::fromUtf8( m_gtd_model.getCaption( uid ).c_str() );
    }

    bool itemContentMatchesString( const std::string &uid, const QString &searchString ) const
    {
        return m_gtd_model.itemContentMatchesString( uid, searchString.toUtf8().constData() );
    }

/// save relevant interface
public:

    std::string createNewInboxItem( const QString &caption )
    {
        return m_gtd_model.createNewInboxItem( caption.toUtf8().constData() );
    }

    void setNote( const std::string &uid, const QString &value )
    {
        m_gtd_model.setNote( uid, value.toUtf8().constData() );
    }

    void plusOne( const std::string &uid )
    {
        m_gtd_model.plusOne( uid );
    }

    void registerItemAsTask( const std::string &task_item, const std::string &project_item )
    {
        m_gtd_model.registerItemAsTask( task_item, project_item );
    }

    void castToProject( const std::string &item )
    {
        m_gtd_model.castToProject( item );
    }

    void setDone( const std::string &task_item )
    {
        m_gtd_model.setDone( task_item );
    }

    void setNextTask( const std::string &project_item, const std::string &task_item )
    {
        return m_gtd_model.setNextTask( project_item, task_item );
    }

    std::string createProject( const QString &project_name )
    {
        return m_gtd_model.createProject( project_name.toUtf8().constData() );
    }

    void setCaption( const std::string &uid, const QString &caption )
    {
        return m_gtd_model.setCaption( uid, caption.toUtf8().constData() );
    }

    void eraseItem( const std::string &uid )
    {
        m_gtd_model.eraseItem( uid );
    }
};

#endif
