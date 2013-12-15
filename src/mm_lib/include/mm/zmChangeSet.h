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

namespace zm
{

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

    JournalItem( const std::string &Uid, ChangeType Type )
        : uid   ( Uid )
        , type  ( Type )
        , time  ( zm::common::time_stamp_iso_ext() )
        , key   ( )
        , value ( )
    {
    }

    std::string uid;
    ChangeType  type;
    std::string time;
    std::string key;
    std::string value;
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


    const std::vector< JournalItem * > & getJournal() const
    {
        return m_journal;
    }

    void add_remove_entry( const std::string &node_uid )
    {}

    void add_item_update(std::vector< JournalItem > changes)
    {}

    void add_item(std::vector< JournalItem > changes)
    {}

    void clear();

private:

    //ChangeSet( const ChangeSet & );
    ChangeSet & operator=( const ChangeSet & );

    void load( const std::string &journalFileName );

    std::vector< JournalItem * > m_journal;

};

}

#endif
