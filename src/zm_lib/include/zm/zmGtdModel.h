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
#include <boost/shared_ptr.hpp>

class zmGtdModel
{
    boost::shared_ptr< zm::MindMatterModel > m_p_things_model;

    zm::uid_t m_item_inbox;
    zm::uid_t m_item_task;
    zm::uid_t m_item_next_task;
    zm::uid_t m_item_project;
    zm::uid_t m_item_group;
    zm::uid_t m_item_done;
    zm::uid_t m_item_knowledge;
    zm::uid_t m_item_person;

/// maintenance interface
public:

    zmGtdModel();

    zmGtdModel(boost::shared_ptr< zm::MindMatterModel > model);

    const boost::shared_ptr< zm::MindMatterModel > const_base() const;

    boost::shared_ptr< zm::MindMatterModel > base();

    void initialize();

/// const interface
public:

    std::string getNote(
            const zm::uid_t &uid ) const;

    int getImportance(
            const zm::uid_t &uid ) const;

    zm::uid_lst_t getInboxItems(
            bool includeDoneItems ) const;

    /// returns a list of UIDs pointing to GTD-Tasks. StandaloneTasks are
    /// included by default, done items are excluded by default
    zm::uid_lst_t getTaskItems(
            bool includeStandaloneTasks,
            bool includeDoneItems ) const;

    zm::uid_lst_t getProjectItems(
            bool includeStandaloneTasks,
            bool includeDoneItems ) const;

    zm::uid_lst_t getDoneItems() const;

    zm::uid_lst_t find(
            const std::string &pattern) const;

    bool isTaskItem(
            const zm::uid_t &item,
            bool includeStandaloneTasks ) const;

    bool isInboxItem(
            const std::string &item ) const;

    bool isProjectItem(
            const zm::uid_t &item,
            bool includeStandaloneTasks ) const;

    bool isDone(
            const zm::uid_t &task_item ) const;

    zm::uid_t getParentProject(
            const zm::uid_t &task_item ) const;

    zm::uid_t getNextTask(
            const zm::uid_t &task_item ) const;

    zm::uid_t orderATask() const;

    bool itemContentMatchesString(
            const zm::uid_t     &uid,
            const std::string   &searchString ) const;

    void print_statistics() const;

    bool empty() const
    {
        /// subtract items automatically created
        return m_p_things_model->getItemCount() == 8;
    }

/// save relevant interface
public:

    zm::uid_t createNewInboxItem(
            const std::string &caption );

    void setNote(
            const zm::uid_t &uid,
            const std::string &value );

    void plusOne(
            const zm::uid_t &uid );

    void registerItemAsTask(
            const zm::uid_t &task_item,
            const zm::uid_t &project_item );

    void setDone(
            const zm::uid_t &task_item );

    void castToProject(
            const zm::uid_t &item );

    void setNextTask(
            const zm::uid_t &project_item,
            const zm::uid_t &task_item );

    zm::uid_t createProject(
            const zm::uid_t &project_name );
};

#endif
