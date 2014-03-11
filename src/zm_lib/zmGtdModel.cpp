/// -*- coding: utf-8 -*-
///
/// file: zmGtdModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/zm/zmGtdModel.h"

#include <mm/zmTrace.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

zm::uid_lst_t zmGtdModel::getInboxItems( bool includeDoneItems ) const
{
    // maybe should be done using http://www.boost.org/doc/libs/1_55_0/libs/iterator/doc/filter_iterator.html

    zm::uid_lst_t l_return;

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type& i,
                  m_p_things_model->things() )
    {
        if( isInboxItem( i.left )
            && (includeDoneItems       || !isDone(  i.left ) ) )
        {
            l_return.push_back( i.left );
        }
    }

    return l_return;
}

zm::uid_lst_t zmGtdModel::getTaskItems( bool includeStandaloneTasks, bool includeDoneItems ) const
{
    zm::uid_lst_t l_return;

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type& i,
                  m_p_things_model->things() )
    {
        if( isTaskItem( i.left, includeStandaloneTasks )
                && (includeDoneItems       || !isDone(  i.left ) ) )
        {
            l_return.push_back( i.left );
        }
    }

    return l_return;
}

zm::uid_lst_t zmGtdModel::getProjectItems( bool includeStandaloneTasks, bool includeDoneItems ) const
{
    zm::uid_lst_t l_return;

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type& i,
                  m_p_things_model->things() )
    {
        if( isProjectItem( i.left, includeStandaloneTasks )
                && (includeDoneItems       || !isDone(  i.left ) ) )
        {
            l_return.push_back( i.left );
        }
    }

    return l_return;
}

zm::uid_lst_t zmGtdModel::getDoneItems() const
{
    zm::uid_lst_t l_return;

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type& i,
                  m_p_things_model->things() )
    {
        if( isDone( i.left ) )
        {
            l_return.push_back( i.left );
        }
    }

    return l_return;
}

std::string zmGtdModel::getNextTask( const std::string &task_item ) const
{
    assert( isProjectItem( task_item, false ) );
    if( !m_p_things_model->hasValue( task_item, "gtd_next_task" ) )
    {
        return "";
    }

    return m_p_things_model->getValue( task_item, "gtd_next_task" );
}

std::string zmGtdModel::orderATask() const
{
    zm::uid_lst_t l_possibleTasks = getTaskItems( true, false );

    int taskId = rand() % l_possibleTasks.size();

    zm::uid_lst_t::const_iterator it = l_possibleTasks.begin();

    for( int i = 0; i < taskId; ++i )
    {
        ++it;
    }

    return *it;
}

bool zmGtdModel::itemContentMatchesString( const std::string &uid, const std::string &searchString ) const
{
    return m_p_things_model->itemContentMatchesString( uid, searchString );
}

void zmGtdModel::print_statistics() const
{
    zm::uid_lst_t l_inbox = getInboxItems( false );
    zm::uid_lst_t l_tasks = getTaskItems( false, false );
    zm::uid_lst_t l_projects = getProjectItems( false, false );
    zm::uid_lst_t l_done = getDoneItems();

    size_t l_total_items = m_p_things_model->getItemCount();

    size_t l_inbox_items = l_inbox.size();
    size_t l_task_items = l_tasks.size();
    size_t l_project_items = l_projects.size();
    size_t l_done_items = l_done.size();

    trace_i( "total items:........ %d", l_total_items );
    trace_i( "gtd items:.......... %d",
                  l_inbox_items + l_project_items + l_task_items + l_done_items );
    trace_i( " gtd inbox items:... %d", l_inbox_items );

    trace_i( " gtd project items:. %d", l_project_items );
    BOOST_FOREACH( std::string s, l_projects )
    {
        trace_i( "    %s: '%s'", s.c_str(), m_p_things_model->getCaption( s ).c_str() );
    }
    trace_i( " gtd task items:.... %d", l_task_items );
    trace_i( " gtd items done:     %d", l_done_items );
}

std::string zmGtdModel::createNewInboxItem( const std::string &caption )
{
    std::string l_item_uid = m_p_things_model->createNewItem( caption );
    trace_i( "GTD: new inbox item %s: '%s'",
                  caption.c_str(),
                  l_item_uid .c_str() );

    m_p_things_model->connectDirected( l_item_uid, m_item_inbox );

    m_p_things_model->persistence_saveLocalModel();

    return l_item_uid;
}

void zmGtdModel::setNote( const std::string &uid, const std::string &value )
{
    trace_i( "GTD: setting note for item %s (%s): '%s'",
                  uid.c_str(),
                  m_p_things_model->getCaption( uid ).c_str(),
                  value.c_str() );

    m_p_things_model->setValue( uid, "gtd_item_note", value );

    m_p_things_model->persistence_saveLocalModel();
}

void zmGtdModel::plusOne( const std::string &uid )
{
    int l_importance = 0;
    if( m_p_things_model->hasValue( uid, "gtd_importance" ) )
    {
        l_importance = boost::lexical_cast<int>( m_p_things_model->getValue( uid, "gtd_importance" ) );
    }
    l_importance += 1;

    m_p_things_model->setValue( uid, "gtd_importance", boost::lexical_cast<std::string>(l_importance) );

    m_p_things_model->persistence_saveLocalModel();
}

void zmGtdModel::registerItemAsTask( const std::string &task_item, const std::string &project_item )
{
    assert( isInboxItem( task_item ) );
    assert( isProjectItem( project_item, false ) );

    m_p_things_model->disconnect( task_item, m_item_inbox );
    m_p_things_model->connectDirected( task_item, m_item_task );
    m_p_things_model->connectDirected( task_item, project_item );

    m_p_things_model->persistence_saveLocalModel();
}

void zmGtdModel::setDone( const std::string &task_item )
{
    m_p_things_model->disconnect( task_item, m_item_inbox );
    m_p_things_model->connectDirected( task_item, m_item_done );

    m_p_things_model->setValue(
                task_item,
                "gtd_time_done",
                zm::common::time_stamp_iso_ext() );

    m_p_things_model->persistence_saveLocalModel();
}

void zmGtdModel::castToProject( const std::string &item )
{
    assert( isInboxItem( item ) );

    m_p_things_model->disconnect( item, m_item_inbox );
    m_p_things_model->connectDirected( item, m_item_project );

    m_p_things_model->persistence_saveLocalModel();
}

void zmGtdModel::setNextTask( const std::string &project_item, const std::string &task_item )
{
    assert( isProjectItem( project_item, false ) );
    assert( isTaskItem( task_item, false ) );

    std::string l_currentNextTask = getNextTask( project_item );
    if( l_currentNextTask != "" )
    {
        m_p_things_model->disconnect( l_currentNextTask, m_item_next_task );
    }
    m_p_things_model->connectDirected( task_item, m_item_next_task );

    m_p_things_model->persistence_saveLocalModel();
}

    std::string zmGtdModel::createProject( const std::string &project_name )
    {
        std::string l_item_uid = m_p_things_model->createNewItem( project_name );

        m_p_things_model->connectDirected( project_name, m_item_project );

        m_p_things_model->persistence_saveLocalModel();

        return l_item_uid;
    }

    int zmGtdModel::getImportance( const std::string &uid ) const
    {
        if( m_p_things_model->hasValue( uid, "gtd_importance" ) )
        {
            return boost::lexical_cast<int>( m_p_things_model->getValue( uid, "gtd_importance" ) );
        }

        return 0;
    }

