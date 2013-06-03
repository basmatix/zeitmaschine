/// -*- coding: utf-8 -*-
///
/// file: zmChangeSet.h
///
/// Copyright (C) 2013 Frans Fuerst
///

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

    bool write( const std::string &journalFileName );

    const std::vector< JournalItem * > & getJournal() const
    {
        return m_journal;
    }

    void push_back( JournalItem *item )
    {
        m_journal.push_back( item );
    }

    void clear();

private:

    ChangeSet( const ChangeSet & );
    ChangeSet & operator=( const ChangeSet & );

    void load( const std::string &journalFileName );

    std::vector< JournalItem * > m_journal;

};

}

#endif
