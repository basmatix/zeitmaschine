/// -*- coding: utf-8 -*-
///
/// file: zmGtdModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/zm/zmGtdModel.h"

#include <mm/zmTrace.h>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

zmGtdModel::zmGtdModel()
    : m_p_things_model  ()
    , m_item_inbox      ()
    , m_item_task       ()
    , m_item_next_task  ()
    , m_item_project    ()
    , m_item_group      ()
    , m_item_done       ()
    , m_item_knowledge  ()
    , m_item_person     ()
{
    m_p_things_model = boost::shared_ptr< zm::MindMatterModel >(
                new zm::MindMatterModel() );
}

zmGtdModel::zmGtdModel(
        boost::shared_ptr< zm::MindMatterModel > model)
    : m_p_things_model  (model)
    , m_item_inbox      ()
    , m_item_task       ()
    , m_item_next_task  ()
    , m_item_project    ()
    , m_item_group      ()
    , m_item_done       ()
    , m_item_knowledge  ()
    , m_item_person     ()
{}


const boost::shared_ptr< zm::MindMatterModel > zmGtdModel::const_base() const
{
    return m_p_things_model;
}


boost::shared_ptr< zm::MindMatterModel > zmGtdModel::base()
{
    return m_p_things_model;
}

void zmGtdModel::initialize()
{
    m_p_things_model->initialize();

    m_item_inbox =     m_p_things_model->findOrCreateTagItem( "gtd_inbox" );
    m_item_task =      m_p_things_model->findOrCreateTagItem( "gtd_task" );
    m_item_next_task = m_p_things_model->findOrCreateTagItem( "gtd_next_task" );
    m_item_project =   m_p_things_model->findOrCreateTagItem( "gtd_project" );
    m_item_group =     m_p_things_model->findOrCreateTagItem( "gtd_group" );
    m_item_done =      m_p_things_model->findOrCreateTagItem( "gtd_done" );
    m_item_knowledge = m_p_things_model->findOrCreateTagItem( "knowledge" );
    m_item_person =    m_p_things_model->findOrCreateTagItem( "person" );

    print_statistics();
}

std::string zmGtdModel::getNote( const std::string &uid ) const
{
    if( uid == "" ) return "";

    if( m_p_things_model->hasValue( uid, "gtd_item_note" ))
    {
        return m_p_things_model->getValue( uid, "gtd_item_note" );
    }
    else
    {
        return "";
    }
}

zm::uid_lst_t zmGtdModel::getInboxItems(
        bool includeDoneItems ) const
{
    // this may look nice but it's crap - future query strings won't look
    // like this

    // query syntax is "interim_filter_tags +gtd_inbox [-gtd_done]"
    std::string l_tag_inbox   = std::string("+") + m_item_inbox;
    std::string l_tag_no_done = std::string("-") + m_item_done;

    return m_p_things_model->query(
                boost::str(boost::format("interim_filter_tags %s %s")
                           % l_tag_inbox
                           % (includeDoneItems ? "":l_tag_no_done)));
}

zm::uid_lst_t zmGtdModel::getTaskItems(
        bool includeStandaloneTasks,
        bool includeDoneItems ) const
{
    // this may look nice but it's crap - future query strings won't look
    // like this

    // query syntax is "interim_filter_tags +gtd_task [-gtd_done] [-gtd_project]"

    std::string l_tag_task       = std::string("+") + m_item_task;
    std::string l_tag_no_done    = std::string("-") + m_item_done;
    std::string l_tag_no_project = std::string("-") + m_item_project;

    return m_p_things_model->query(
                boost::str(boost::format("interim_filter_tags %s %s %s")
                           % l_tag_task
                           % (includeDoneItems       ? "":l_tag_no_done)
                           % (includeStandaloneTasks ? "":l_tag_no_project)));
}

zm::uid_lst_t zmGtdModel::getProjectItems(
        bool includeStandaloneTasks,
        bool includeDoneItems ) const
{
    // this may look nice but it's crap - future query strings won't look
    // like this

    // query syntax is "interim_filter_tags +gtd_project [-gtd_done] [-gtd_task]"

    std::string l_tag_project = std::string("+") + m_item_project;
    std::string l_tag_no_done = std::string("-") + m_item_done;
    std::string l_tag_no_task = std::string("-") + m_item_task;

    return m_p_things_model->query(
                boost::str(boost::format("interim_filter_tags %s %s %s")
                           % l_tag_project
                           % (includeDoneItems       ? "":l_tag_no_done)
                           % (includeStandaloneTasks ? "":l_tag_no_task)));
}

zm::uid_lst_t zmGtdModel::getDoneItems() const
{
    // this may look nice but it's crap - future query strings won't look
    // like this

    // query syntax is "interim_filter_tags +gtd_done"

    std::string l_tag_done = std::string("+") + m_item_done;

    return m_p_things_model->query(
                boost::str(boost::format("interim_filter_tags %s")
                           % l_tag_done));
}

bool zmGtdModel::isTaskItem(
        const std::string &item,
        bool includeStandaloneTasks ) const
{
    if( !includeStandaloneTasks && m_p_things_model->hasTag( item, m_item_project ) )
    {
        return false;
    }
    return m_p_things_model->hasTag( item, "gtd_task" );
}

bool zmGtdModel::isInboxItem(
        const std::string &item ) const
{
    return m_p_things_model->isConnected( item, m_item_inbox );
    // should be equal to
    //return m_p_things_model->hasTag( item, "gtd_inbox" );
}

bool zmGtdModel::isProjectItem(
        const std::string &item,
        bool includeStandaloneTasks ) const
{
    if( !includeStandaloneTasks && m_p_things_model->hasTag( item, "gtd_task" ) )
    {
        return false;
    }
    return m_p_things_model->hasTag( item, "gtd_project" );
}

bool zmGtdModel::isDone(
        const std::string &task_item ) const
{
    return m_p_things_model->hasTag( task_item, "gtd_done" );
}

zm::uid_t zmGtdModel::getParentProject(
        const std::string &task_item ) const
{
    assert( isTaskItem( task_item, false ) );
    assert( m_p_things_model->hasValue( task_item, "gtd_parent_project" ) );

    return m_p_things_model->getValue( task_item, "gtd_parent_project" );
}

zm::uid_t zmGtdModel::getNextTask( const std::string &task_item ) const
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

bool zmGtdModel::itemContentMatchesString(
        const std::string &uid,
        const std::string &searchString ) const
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
    for( std::string s: l_projects )
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

    return l_item_uid;
}

void zmGtdModel::setNote( const std::string &uid, const std::string &value )
{
    trace_i( "GTD: setting note for item %s (%s): '%s'",
                  uid.c_str(),
                  m_p_things_model->getCaption( uid ).c_str(),
                  value.c_str() );

    m_p_things_model->setValue( uid, "gtd_item_note", value );
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
}

void zmGtdModel::registerItemAsTask( const std::string &task_item, const std::string &project_item )
{
    assert( isInboxItem( task_item ) );
    assert( isProjectItem( project_item, false ) );

    m_p_things_model->disconnect( task_item, m_item_inbox );
    m_p_things_model->connectDirected( task_item, m_item_task );
    m_p_things_model->connectDirected( task_item, project_item );
}

void zmGtdModel::setDone( const std::string &task_item )
{
    m_p_things_model->disconnect( task_item, m_item_inbox );
    m_p_things_model->connectDirected( task_item, m_item_done );

    m_p_things_model->setValue(
                task_item,
                "gtd_time_done",
                zm::common::time_stamp_iso_ext() );
}

void zmGtdModel::castToProject( const std::string &item )
{
    assert( isInboxItem( item ) );

    m_p_things_model->disconnect( item, m_item_inbox );
    m_p_things_model->connectDirected( item, m_item_project );
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
}

std::string zmGtdModel::createProject( const std::string &project_name )
{
    std::string l_item_uid = m_p_things_model->createNewItem( project_name );

    m_p_things_model->connectDirected( project_name, m_item_project );

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

