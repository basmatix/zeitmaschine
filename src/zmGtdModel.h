/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMGTDMODEL_H
#define ZMGTDMODEL_H

#include <zmModel.h>
#include "zmTrace.h"

class zmGtdModel
{
    ThingsModel m_things_model;

///
/// GTD specific interface
///
public:

    std::string createNewInboxItem( const std::string &caption )
    {
        std::string l_item_uid = m_things_model.createNewItem( caption );
        tracemessage( "GTD: new inbox item %s: '%s'",
                      caption.c_str(),
                      l_item_uid .c_str() );

        m_things_model.addAttribute( l_item_uid, "gtd_item_unhandled" );

        return l_item_uid;
    }

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

    void setNote( const std::string &uid, const std::string &value )
    {
        tracemessage( "GTD: setting note for item %s (%s): '%s'",
                      uid.c_str(),
                      m_things_model.getCaption( uid ).c_str(),
                      value.c_str() );

        m_things_model.setValue( uid, "gtd_item_note", value );
    }

    void registerItemAsTask( const std::string &task_item, const std::string &project_item )
    {
        assert( isInboxItem( task_item ) );
        assert( isProjectItem( project_item ) );

        m_things_model.removeAttribute( task_item, "gtd_item_unhandled" );
        m_things_model.addAttribute( task_item, "gtd_task" );
        m_things_model.setValue( task_item, "gtd_parent_project", project_item );
    }

    std::list< std::string > getInboxItems( bool includeDoneItems ) const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isInboxItem( i.first )
                && (includeDoneItems       || !isDone(  i.first ) ) )
            {
                l_return.push_back( i.first );
            }
        }

        return l_return;
    }

    /// returns a list of UIDs pointing to GTD-Tasks. StandaloneTasks are included by default,
    /// done items are excluded by default
    std::list< std::string > getTaskItems( bool includeStandaloneTasks, bool includeDoneItems ) const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isTaskItem( i.first )
                    && (includeStandaloneTasks || !isProjectItem(  i.first ) )
                    && (includeDoneItems       || !isDone(  i.first ) ) )
            {
                l_return.push_back( i.first );
            }
        }

        return l_return;
    }

    std::list< std::string > getProjectItems( bool includeStandaloneTasks, bool includeDoneItems ) const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isProjectItem( i.first )
                    && (includeStandaloneTasks || !isTaskItem(  i.first ) )
                    && (includeDoneItems       || !isDone(  i.first ) ) )
            {
                l_return.push_back( i.first );
            }        }

        return l_return;
    }

    std::list< std::string > getDoneItems() const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isDone(  i.first ) )
            {
                l_return.push_back( i.first );
            }
        }

        return l_return;
    }

    void setDone( const std::string &task_item )
    {
        m_things_model.removeAttribute(
                    task_item, "gtd_item_unhandled" );

        m_things_model.addAttribute(
                    task_item, "gtd_item_done" );

        m_things_model.setValue(
                    task_item, "gtd_time_done", ThingsModel::time_stamp() );
    }

    void castToProject( const std::string &item )
    {
        assert( isInboxItem( item ) );
        m_things_model.removeAttribute( item, "gtd_item_unhandled" );
        m_things_model.addAttribute( item, "gtd_project" );
    }

    bool isTaskItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_task" );
    }

    bool isInboxItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_item_unhandled" );
    }

    bool isProjectItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_project" );
    }

    bool isDone( const std::string &task_item ) const
    {
        return m_things_model.hasAttribute( task_item, "gtd_item_done" );
    }

    std::string getParentProject( const std::string &task_item )
    {
        assert( isTaskItem( task_item ) );
        assert( m_things_model.hasValue( task_item, "gtd_parent_project" ) );

        return m_things_model.getValue( task_item, "gtd_parent_project" );
    }

    std::string createProject( const std::string &project_name )
    {
        std::string l_item_uid = m_things_model.createNewItem( project_name );

        m_things_model.addAttribute( l_item_uid, "gtd_project" );

        return l_item_uid;
    }

    std::string orderATask() const
    {
        std::list< std::string > l_possibleTasks = getTaskItems( true, false );

        int taskId = rand() % l_possibleTasks.size();

        std::list< std::string >::const_iterator it = l_possibleTasks.begin();

        for( int i = 0; i < taskId; ++i )
        {
            ++it;
        }

        return *it;
    }

    void print_statistics()
    {
        std::list< std::string > l_inbox = getInboxItems( false );
        std::list< std::string > l_tasks = getTaskItems( false, false );
        std::list< std::string > l_projects = getProjectItems( false, false );
        std::list< std::string > l_done = getDoneItems();

        size_t l_total_items = m_things_model.getItemCount();

        size_t l_inbox_items = l_inbox.size();
        size_t l_task_items = l_tasks.size();
        size_t l_project_items = l_projects.size();
        size_t l_done_items = l_done.size();

        tracemessage( "total items:........ %d", l_total_items );
        tracemessage( "gtd items:.......... %d",
                      l_inbox_items + l_project_items + l_task_items + l_done_items );
        tracemessage( " gtd inbox items:... %d", l_inbox_items );

        tracemessage( " gtd project items:. %d", l_project_items );
        BOOST_FOREACH( std::string s, l_projects )
        {
            tracemessage( "    %s: '%s'", s.c_str(), m_things_model.getCaption( s ).c_str() );
        }
        tracemessage( " gtd task items:.... %d", l_task_items );
        tracemessage( " gtd items done:     %d", l_done_items );
    }

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
        return m_things_model.setCaption( uid, caption );
    }

    void load()
    {
        m_things_model.load();
        print_statistics();
    }

    void load( const std::string &filename )
    {
        m_things_model.load( filename );
        print_statistics();
    }

    void save( const std::string &filename )
    {
        return m_things_model.save( filename );
    }

    void eraseItem( const std::string &uid )
    {
        return m_things_model.eraseItem( uid );
    }

    bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const
    {
        return m_things_model.itemContentMatchesString( uid, searchString );
    }
};

#endif
