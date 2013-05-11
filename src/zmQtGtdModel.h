/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMQTGTDADAPTER_H
#define ZMQTGTDADAPTER_H

#include <zmGtdModel.h>
#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>

/// std-c++/Qt - interface to zmGtdModel. Aims to be a
/// fully qualified Qt model in the future
class zmQtGtdModel
    : public QAbstractItemModel
{
    zmGtdModel m_gtd_model;

/// maintenance interface
public:

    zmQtGtdModel()
        : m_gtd_model()
    {}

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
    QModelIndex index(int, int, const QModelIndex&) const
    {
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex&) const
    {
        return QModelIndex();
    }

    int rowCount(const QModelIndex&) const
    {
        return 0;
    }

    int columnCount(const QModelIndex&) const
    {
        return 0;
    }

    QVariant data(const QModelIndex&, int) const
    {
        return QVariant();
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
