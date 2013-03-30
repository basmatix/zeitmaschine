/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMQTGTDADAPTER_H
#define ZMQTGTDADAPTER_H

#include <zmGtdModel.h>
#include <QtCore/QString>

class zmQtGtdModel
{
    zmGtdModel m_gtd_model;

///
/// GTD specific interface
///
public:

    std::string createNewInboxItem( const QString &caption )
    {
        return  m_gtd_model.createNewInboxItem( caption.toUtf8().constData() );
    }

    QString getNote( const std::string &uid ) const
    {
        return QString::fromUtf8( m_gtd_model.getNote( uid ).c_str() );
    }

    std::time_t getCreationTime( const std::string &uid ) const
    {
        return m_gtd_model.getCreationTime( uid );
    }

    void setNote( const std::string &uid, const QString &value )
    {
        m_gtd_model.setNote( uid, value.toUtf8().constData() );
    }

    void registerItemAsTask( const std::string &task_item, const std::string &project_item )
    {
        m_gtd_model.registerItemAsTask( task_item, project_item );
    }

    std::list< std::string > getInboxItems() const
    {
        return m_gtd_model.getInboxItems();
    }

    std::list< std::string > getTaskItems() const
    {
        return m_gtd_model.getTaskItems();
    }

    std::list< std::string > getProjectItems() const
    {
        return m_gtd_model.getProjectItems();
    }

    void castToProject( const std::string &item )
    {
        m_gtd_model.castToProject( item );
    }

    void setDone( const std::string &task_item )
    {
        m_gtd_model.setDone( task_item );
    }

    bool isTaskItem( const std::string &item ) const
    {
        return m_gtd_model.isTaskItem( item );
    }

    bool isInboxItem( const std::string &item ) const
    {
        return m_gtd_model.isInboxItem( item );
    }

    bool isProjectItem( const std::string &item ) const
    {
        return m_gtd_model.isProjectItem( item );
    }

    bool isDone( const std::string &task_item ) const
    {
        return m_gtd_model.isDone( task_item );
    }

    std::string getParentProject( const std::string &task_item )
    {
        return m_gtd_model.getParentProject( task_item );
    }

    std::string createProject( const QString &project_name )
    {
        return m_gtd_model.createProject( project_name.toUtf8().constData() );
    }

    std::string orderATask() const
    {
        return m_gtd_model.orderATask();
    }

    const QString getCaption( const std::string &uid ) const
    {
        return QString::fromUtf8( m_gtd_model.getCaption( uid ).c_str() );
    }

    void setCaption( const std::string &uid, const QString &caption )
    {
        return m_gtd_model.setCaption( uid, caption.toUtf8().constData() );
    }

    void load()
    {
        m_gtd_model.load();
    }

    void load( const std::string &filename )
    {
        m_gtd_model.load( filename );
    }

    void save( const std::string &filename )
    {
        m_gtd_model.save( filename );
    }

    void eraseItem( const std::string &uid )
    {
        m_gtd_model.eraseItem( uid );
    }

    bool itemContentMatchesString( const std::string &uid, const QString &searchString ) const
    {
        return m_gtd_model.itemContentMatchesString( uid, searchString.toUtf8().constData() );
    }
};

#endif
