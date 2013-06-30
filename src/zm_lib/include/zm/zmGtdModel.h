/// -*- coding: utf-8 -*-
///
/// file: zmGtdModel.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMGTDMODEL_H
#define ZMGTDMODEL_H

#include <mm/zmModel.h>

#include <list>


#include <assert.h>

class zmGtdModel
{
    zm::MindMatterModel m_things_model;
    std::string m_item_inbox;
    std::string m_item_task;
    std::string m_item_next_task;
    std::string m_item_project;
    std::string m_item_group;
    std::string m_item_done;
    std::string m_item_knowledge;
    std::string m_item_person;

/// maintenance interface
public:

    zmGtdModel()
        : m_things_model    ()
        , m_item_inbox      ()
        , m_item_task       ()
        , m_item_next_task  ()
        , m_item_project    ()
        , m_item_group      ()
        , m_item_done       ()
        , m_item_knowledge  ()
        , m_item_person     ()
    {}

    bool hasUsedUsername() const
    {
        return m_things_model.hasUsedUsername();
    }

    bool hasUsedHostname() const
    {
        return m_things_model.hasUsedHostname();
    }

    void setUsedUsername( const std::string &username )
    {
        m_things_model.setUsedUsername( username );
    }

    void setUsedHostname( const std::string &hostname )
    {
        m_things_model.setUsedHostname( hostname );
    }

    void setConfigPersistance( bool value )
    {
        m_things_model.setConfigPersistance( value );
    }

    void setLocalFolder( const std::string &path )
    {
        m_things_model.setLocalFolder( path );
    }

    void addDomainSyncFolder( const std::string &domainName, const std::string &path )
    {
        m_things_model.addDomainSyncFolder( domainName, path );
    }

    void initialize()
    {
        m_things_model.initialize();
        m_item_inbox =      m_things_model.findOrCreateTagItem( "gtd_inbox" );
        m_item_task =       m_things_model.findOrCreateTagItem( "gtd_task" );
        m_item_next_task =  m_things_model.findOrCreateTagItem( "gtd_next_task" );
        m_item_project =    m_things_model.findOrCreateTagItem( "gtd_project" );
        m_item_group =      m_things_model.findOrCreateTagItem( "gtd_group" );
        m_item_done =       m_things_model.findOrCreateTagItem( "gtd_done" );
        m_item_knowledge =  m_things_model.findOrCreateTagItem( "knowledge" );
        m_item_person =     m_things_model.findOrCreateTagItem( "person" );

        print_statistics();
    }

    void sync()
    {
        m_things_model.sync();
    }

    void merge( const std::string &modelFile )
    {
        m_things_model.merge( modelFile );
    }

/// const interface
public:

    std::string getNote( const std::string &uid ) const
    {
        if( uid == "" ) return "";

        if( m_things_model.hasValue( uid, "gtd_item_note" ))
        {
            return m_things_model.getValue( uid, "gtd_item_note" );
        }
        else
        {
            return "";
        }
    }

    int getImportance( const std::string &uid ) const;

    std::list< std::string > getInboxItems( bool includeDoneItems ) const;

    /// returns a list of UIDs pointing to GTD-Tasks. StandaloneTasks are included by default,
    /// done items are excluded by default
    std::list< std::string > getTaskItems( bool includeStandaloneTasks, bool includeDoneItems ) const;

    std::list< std::string > getProjectItems( bool includeStandaloneTasks, bool includeDoneItems ) const;

    std::list< std::string > getDoneItems() const;

    bool isTaskItem( const std::string &item, bool includeStandaloneTasks ) const
    {
        if( !includeStandaloneTasks && m_things_model.hasAttribute( item, "gtd_project" ) )
        {
            return false;
        }
        return m_things_model.hasAttribute( item, "gtd_task" );
    }

    bool isInboxItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_item_unhandled" );
    }

    bool isProjectItem( const std::string &item, bool includeStandaloneTasks ) const
    {
        if( !includeStandaloneTasks && m_things_model.hasAttribute( item, "gtd_task" ) )
        {
            return false;
        }
        return m_things_model.hasAttribute( item, "gtd_project" );
    }

    bool isDone( const std::string &task_item ) const
    {
        return m_things_model.hasAttribute( task_item, "gtd_item_done" );
    }

    std::string getParentProject( const std::string &task_item ) const
    {
        assert( isTaskItem( task_item, false ) );
        assert( m_things_model.hasValue( task_item, "gtd_parent_project" ) );

        return m_things_model.getValue( task_item, "gtd_parent_project" );
    }

    std::string getNextTask( const std::string &task_item ) const;

    std::string orderATask() const;

    bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const;

    void print_statistics() const;

    bool empty() const
    {
        return m_things_model.getItemCount() == 0;
    }

/// save relevant interface
public:

    std::string createNewInboxItem( const std::string &caption );

    void setNote( const std::string &uid, const std::string &value );

    void plusOne( const std::string &uid );

    void registerItemAsTask( const std::string &task_item, const std::string &project_item );

    void setDone( const std::string &task_item );

    void castToProject( const std::string &item );

    void setNextTask( const std::string &project_item, const std::string &task_item );

    std::string createProject( const std::string &project_name );

///
/// low level interface - to be vanished
///
public:

    std::time_t getCreationTime( const std::string &uid ) const
    {
        return m_things_model.getCreationTime( uid );
    }

    const std::string & getCaption( const std::string &uid ) const
    {
        return m_things_model.getCaption( uid );
    }

    void setCaption( const std::string &uid, const std::string &caption )
    {
        m_things_model.setCaption( uid, caption );

        m_things_model.localSave();
    }

    //void save( const std::string &filename )
    //{
    //    return m_things_model.save( filename );
    //}

    void eraseItem( const std::string &uid )
    {
        m_things_model.eraseItem( uid );

        m_things_model.localSave();
    }
};

#endif