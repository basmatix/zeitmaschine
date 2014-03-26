/// -*- coding: utf-8 -*-
///
/// file: zmQtGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMQTGTDMODEL_H
#define ZMQTGTDMODEL_H

#include <zm/zmGtdModel.h>

#include <QtCore/QString>

class zmWidgetItemMap;
class QTreeWidgetItem;

/// std-c++/Qt - interface to zmGtdModel. Aims to be a
/// fully qualified Qt model in the future
class zmQtGtdModel
{
    zmQtGtdModel( const zmQtGtdModel & );
    zmQtGtdModel &operator=( const zmQtGtdModel & );

    zmGtdModel m_gtd_model;

/// maintenance interface
public:

    zmWidgetItemMap *m_wi_map;
    QTreeWidgetItem *m_liToday;
    QTreeWidgetItem *m_liInbox;
    QTreeWidgetItem *m_liProjects;
    QTreeWidgetItem *m_liContexts;
    QTreeWidgetItem *m_liDone;

    zmQtGtdModel();
    virtual ~zmQtGtdModel();

    bool hasUsedUsername() const
    {
        return m_gtd_model.base()->hasUsedUsername();
    }

    bool hasUsedHostname() const
    {
        return m_gtd_model.base()->hasUsedHostname();
    }

    void setUsedUsername( const QString &username )
    {
        m_gtd_model.base()->setUsedUsername( username.toUtf8().constData() );
    }

    void setUsedHostname( const QString &hostname )
    {
        m_gtd_model.base()->setUsedHostname( hostname.toUtf8().constData() );
    }

    void setConfigpersistence( bool value )
    {
        m_gtd_model.base()->setConfigpersistence( value );
    }

    void setLocalFolder( const QString &path )
    {
        m_gtd_model.base()->setLocalFolder( path.toUtf8().constData() );
    }

    void addDomainSyncFolder( const QString &domainName, const QString &path )
    {
        m_gtd_model.base()->addDomainSyncFolder( domainName.toUtf8().constData(), path.toUtf8().constData() );
    }

    void initialize( QTreeWidgetItem *root );

    void addListItem( const std::string uid );

    void populate();

    void sync()
    {
        m_gtd_model.base()->persistence_sync();
    }

    void createSnapshot()
    {
        m_gtd_model.base()->persistence_createSnapshot();
    }

    void localSave( const std::string &filename = "" )
    {
        m_gtd_model.base()->persistence_saveLocalModel();
    }

/// const interface
public:

    QString getNote( const std::string &uid ) const
    {
        return QString::fromUtf8( m_gtd_model.getNote( uid ).c_str() );
    }

    std::time_t getCreationTime( const std::string &uid ) const
    {
        return m_gtd_model.base()->getCreationTime( uid );
    }

    int getImportance( const std::string &uid ) const
    {
        return m_gtd_model.getImportance( uid );
    }

    zm::uid_lst_t getInboxItems( bool includeDoneItems ) const
    {
        return m_gtd_model.getInboxItems( includeDoneItems );
    }

    zm::uid_lst_t getDoneItems() const
    {
        return m_gtd_model.getDoneItems();
    }

    zm::uid_lst_t getTaskItems( bool includeStandaloneTasks, bool includeDoneItems ) const
    {
        return m_gtd_model.getTaskItems( includeStandaloneTasks, includeDoneItems );
    }

    zm::uid_lst_t getProjectItems( bool includeStandaloneTasks, bool includeDoneItems ) const
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
        return QString::fromUtf8( m_gtd_model.base()->getCaption( uid ).c_str() );
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
        return m_gtd_model.base()->setCaption( uid, caption.toUtf8().constData() );
    }

    void eraseItem( const std::string &uid )
    {
        m_gtd_model.base()->eraseItem( uid );
    }
};

#endif
