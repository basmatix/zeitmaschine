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

    std::list< std::string > getInboxItems() const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isInboxItem( i.first ))
            l_return.push_back( i.first );
        }

        return l_return;
    }

    std::list< std::string > getTaskItems() const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isTaskItem( i.first ))
            l_return.push_back( i.first );
        }

        return l_return;
    }

    std::list< std::string > getProjectItems() const
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isProjectItem( i.first ))
            l_return.push_back( i.first );
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
        std::list< std::string > l_possibleTasks = getTaskItems();

        int taskId = rand() % l_possibleTasks.size();

        std::list< std::string >::const_iterator it = l_possibleTasks.begin();

        for( int i = 0; i < taskId; ++i )
        {
            ++it;
        }

        return *it;
    }

///
/// low level interface - to be vanished
///
public:

    const std::string & getCaption( const std::string &uid ) const
    {
        return m_things_model.getCaption( uid );
    }

    void setCaption( const std::string &uid, const std::string &caption )
    {
        return m_things_model.setCaption( uid, caption );
    }

    void load( const std::string &filename )
    {
        m_things_model.load( filename );
        size_t l_total_items = m_things_model.getItemCount();
        size_t l_inbox_items = getInboxItems().size();
        size_t l_task_items = getTaskItems().size();
        size_t l_project_items = getProjectItems().size();
        size_t l_done_items = 0;

        tracemessage( "total items:........ %d", l_total_items );
        tracemessage( "gtd items:.......... %d",
                      l_inbox_items + l_project_items + l_task_items);
        tracemessage( " gtd inbox items:... %d", l_inbox_items );
        tracemessage( " gtd project items:. %d", l_project_items );
        tracemessage( " gtd task items:.... %d", l_task_items );
        tracemessage( " gtd items done:     %d", l_done_items );

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
