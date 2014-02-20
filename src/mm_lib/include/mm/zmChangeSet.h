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

#include "zmTypes.h"

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

namespace zm
{

class JournalItem;
class MindMatter;

typedef std::list< boost::shared_ptr<JournalItem> >   journal_item_vec_t;
typedef boost::shared_ptr< JournalItem >                journal_ptr_t;

class JournalItem
{
public:
    enum ChangeType
    {
        CreateItem,
        EraseItem,
        SetStringValue,
        AddAttribute, //deprecated
//        RemoveAttribute,
        ChangeCaption,
        Connect,
        Disconnect
    };

    static journal_ptr_t createCreate(
            const std::string   &item_uid,
            const std::string   &caption )
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, CreateItem));
        l_result->value = caption;
        return l_result;
    }

    static journal_ptr_t createSetStringValue(
            const std::string   &item_uid,
            const std::string   &name,
            const std::string   &value)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, SetStringValue));
        l_result->key = name;
        l_result->value = value;
        return l_result;
    }

    static journal_ptr_t createErase(
            const std::string   &item_uid)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, EraseItem));
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

//    static journal_ptr_t createRemoveAttribute(
//            const std::string &Uid,
//            const std::string &name)
//    {
//        journal_ptr_t l_result =
//                journal_ptr_t(new JournalItem(Uid, RemoveAttribute));
//        l_result->key = name;
//        return l_result;
//    }

    static journal_ptr_t createChangeCaption(
            const std::string   &item_uid,
            const std::string   &value)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, ChangeCaption));
        l_result->value = value;
        return l_result;
    }

    static journal_ptr_t createConnect(
            const std::string &item_uid,
            const neighbour_t &other_item_uid)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, Connect));
        l_result->key = other_item_uid.first;
        l_result->value = std::to_string(other_item_uid.second);
        return l_result;
    }

    static journal_ptr_t createDisconnect(
            const std::string &item_uid,
            const std::string &other_item_uid)
    {
        journal_ptr_t l_result =
                journal_ptr_t(new JournalItem(item_uid, Disconnect));
        l_result->value = other_item_uid;
        return l_result;
    }

    std::string       item_uid;
    ChangeType        type;
    std::string       time;
    std::string       key;
    std::string       value;

private:

    JournalItem( const std::string &item_uid, ChangeType Type )
        : item_uid      ( item_uid )
        , type          ( Type )
        , time          ( zm::common::time_stamp_iso_ext() )
        , key           ( )
        , value         ( )
    {
    }

    JournalItem(const JournalItem &);
    JournalItem & operator=( const JournalItem & );
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
    {
        m_journal.push_back( JournalItem::createErase(node_uid));
    }

    void append(journal_item_vec_t changes);

    void clear();

    bool isEmpty() const
    {
        return m_journal.empty();
    }

    size_t size() const
    {
        return m_journal.size();
    }

    void debug_dump() const;

private:

    //ChangeSet( const ChangeSet & );
    //ChangeSet & operator=( const ChangeSet & );

    void load( const std::string &journalFileName );

    journal_item_vec_t m_journal;

};

}

#endif
