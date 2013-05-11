/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMQTGTDADAPTER_H
#define ZMQTGTDADAPTER_H

#include <zmQtGtdItem.h>

#include <zmGtdModel.h>
#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>


/// std-c++/Qt - interface to zmGtdModel. Aims to be a
/// fully qualified Qt model in the future
class zmQtGtdModel
    : public QAbstractItemModel
{
    zmGtdModel   m_gtd_model;
    zmQtGtdItem *m_rootItem;

    zmQtGtdModel( const zmQtGtdModel & );
    zmQtGtdModel & operator= ( const zmQtGtdModel & );

/// maintenance interface
public:

    zmQtGtdModel()
        : m_gtd_model   ()
        , m_rootItem    ( NULL )
    {
        QList< QVariant > l_rootData;
        l_rootData << "Title" << "Summary";
        m_rootItem = new zmQtGtdItem( l_rootData );
    }

    virtual ~zmQtGtdModel()
    {
        delete m_rootItem;
    }

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
    Qt::ItemFlags flags( const QModelIndex &index ) const
    {
        if (!index.isValid())
        {
            return 0;
        }

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    QVariant headerData(int section, Qt::Orientation orientation,
                                   int role) const
    {
        if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            return m_rootItem->data( section );
        }

        return QVariant();
    }

    QModelIndex index( int row, int column, const QModelIndex &parent ) const
    {
        if( !hasIndex(row, column, parent) )
        {
            return QModelIndex();
        }

        zmQtGtdItem *l_parentItem;

        if( !parent.isValid() )
        {
            l_parentItem = m_rootItem;
        }
        else
        {
            l_parentItem = static_cast<zmQtGtdItem*>(parent.internalPointer());
        }

        zmQtGtdItem *l_childItem = l_parentItem->child(row);
        if( l_childItem == NULL )
        {
            return QModelIndex();
        }

        return createIndex(row, column, l_childItem);
    }

    QModelIndex parent( const QModelIndex &index ) const
    {
        if( !index.isValid() )
        {
            return QModelIndex();
        }

        zmQtGtdItem *l_childItem = static_cast< zmQtGtdItem * >( index.internalPointer() );
        zmQtGtdItem *l_parentItem = l_childItem->parent();

        if( l_parentItem == m_rootItem )
        {
            return QModelIndex();
        }

        return createIndex(l_parentItem->row(), 0, l_parentItem);
    }

    int rowCount( const QModelIndex &parent ) const
    {
        if( parent.column() > 0 )
        {
            return 0;
        }

        zmQtGtdItem *l_parentItem;

        if (!parent.isValid())
        {
            l_parentItem = m_rootItem;
        }
        else
        {
            l_parentItem = static_cast< zmQtGtdItem * >(parent.internalPointer());
        }

        return l_parentItem->childCount();
    }

    int columnCount( const QModelIndex &parent ) const
    {
        if( parent.isValid() )
        {
            return static_cast< zmQtGtdItem* >( parent.internalPointer() )->columnCount();
        }
        else
        {
            return m_rootItem->columnCount();
        }
    }

    QVariant data( const QModelIndex &index, int role ) const
    {
        if (!index.isValid())
        {
            return QVariant();
        }

        if (role != Qt::DisplayRole)
        {
            return QVariant();
        }

        zmQtGtdItem *l_item = static_cast< zmQtGtdItem * >(index.internalPointer());

        return l_item->data( index.column() );
    }

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
