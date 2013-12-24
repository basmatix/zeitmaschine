/// -*- coding: utf-8 -*-
///
/// file: zmChangeSet.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#pragma once
#ifndef ZMCHANGESET_H
#define ZMCHANGESET_H

#include "zmCommon.h"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace zm
{

class JournalItem;
typedef std::vector< boost::shared_ptr<JournalItem> > journal_item_vec_t;
typedef boost::shared_ptr< JournalItem > journal_ptr_t;

class JournalItem
{
public:
    enum ChangeType
    {
        CreateItem,
        SetStringValue,
        EraseItem,
        AddAttribute,
        RemoveAttribute,
        ChangeCaption,
        Connect
    };

    static journal_ptr_t createCreate(
            const std::string &Uid,
            const std::string &caption )
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, CreateItem));
        l_result->key = caption;
        return l_result;
    }

    static journal_ptr_t createSetStringValue(
            const std::string &Uid,
            const std::string &name,
            const std::string &value)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, SetStringValue));
        l_result->key = name;
        l_result->value = value;
        return l_result;
    }

    static journal_ptr_t createErase(
            const std::string &Uid)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, EraseItem));
        return l_result;
    }

    static journal_ptr_t createAddAttribute(
            const std::string &Uid,
            const std::string &name)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, AddAttribute));
        l_result->key = name;
        return l_result;
    }

    static journal_ptr_t createRemoveAttribute(
            const std::string &Uid,
            const std::string &name)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, RemoveAttribute));
        l_result->key = name;
        return l_result;
    }

    static journal_ptr_t createChangeCaption(
            const std::string &Uid,
            const std::string &value)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(Uid, ChangeCaption));
        l_result->value = value;
        return l_result;
    }

    std::string uid;
    ChangeType  type;
    std::string time;
    std::string key;
    std::string value;
private:
    JournalItem( const std::string &Uid, ChangeType Type )
        : uid   ( Uid )
        , type  ( Type )
        , time  ( zm::common::time_stamp_iso_ext() )
        , key   ( )
        , value ( )
    {
    }
};

/// classifies difference from one model to another. Can be constructed
/// from two models and can be applied to the first model to gain the
/// second one
class ChangeSet
{

public:

    ChangeSet()
        : m_journal()
    {}

    ChangeSet( const std::string &journalFileName )
        : m_journal()
    {
        load( journalFileName );
    }

    virtual ~ChangeSet(){}

    /// serializes to a file
    bool write( const std::string &journalFileName );

    const journal_item_vec_t & getJournal() const
    {
        return m_journal;
    }

    void add_remove_entry( const std::string node_uid )
    {}

    void add_item_update(journal_item_vec_t changes)
    {}

    void add_item(journal_item_vec_t changes)
    {}

    void clear();

private:

    //ChangeSet( const ChangeSet & );
    ChangeSet & operator=( const ChangeSet & );

    void load( const std::string &journalFileName );

    journal_item_vec_t m_journal;

};

}

#endif
