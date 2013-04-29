/// copyright (C) 2013 Frans Fuerst
/// -*- coding: utf-8 -*-

#ifndef ZMCHANGESET_H
#define ZMCHANGESET_H

//#include "zmTrace.h"
//#include "osal.h"
#include "zmCommon.h"

//#include <map>
//#include <set>
//#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

//#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
//#include <boost/date_time.hpp>
//#include <boost/functional/hash.hpp>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

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
        ChangeCaption
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

    void write( const std::string &journalFileName )
    {
        std::ofstream l_fout( journalFileName.c_str() );

        assert( l_fout.is_open() );

        if( m_journal.empty() )
        {
            return;
        }

        /// NOTE: we do the yaml exporting here semi manually
        ///       because this way we can ensure consistent
        ///       order of items between savings

        YAML::Emitter l_yaml_emitter( l_fout );

        l_yaml_emitter << YAML::BeginSeq;

        BOOST_FOREACH(const JournalItem * j, m_journal)
        {
            l_yaml_emitter << YAML::BeginMap;

            l_yaml_emitter << YAML::Key << "update";
            l_yaml_emitter << YAML::Value << j->uid;

            l_yaml_emitter << YAML::Key << "time";
            l_yaml_emitter << YAML::Value << j->time;

            l_yaml_emitter << YAML::Key << "type";
            switch( j->type )
            {
            case JournalItem::CreateItem:
                l_yaml_emitter << YAML::Value << "CreateItem";
                l_yaml_emitter << YAML::Key << "caption";
                l_yaml_emitter << YAML::Value << j->value;
                break;
            case JournalItem::SetStringValue:
                l_yaml_emitter << YAML::Value << "SetStringValue";
                l_yaml_emitter << YAML::Key << "name";
                l_yaml_emitter << YAML::Value << j->key;
                l_yaml_emitter << YAML::Key << "value";
                l_yaml_emitter << YAML::Value << j->value;
                break;
            case JournalItem::EraseItem:
                l_yaml_emitter << YAML::Value << "EraseItem";
                break;
            case JournalItem::AddAttribute:
                l_yaml_emitter << YAML::Value << "AddAttribute";
                l_yaml_emitter << YAML::Key << "name";
                l_yaml_emitter << YAML::Value << j->key;
                break;
            case JournalItem::RemoveAttribute:
                l_yaml_emitter << YAML::Value << "RemoveAttribute";
                l_yaml_emitter << YAML::Key << "name";
                l_yaml_emitter << YAML::Value << j->key;
                break;
            case JournalItem::ChangeCaption:
                l_yaml_emitter << YAML::Value << "ChangeCaption";
                l_yaml_emitter << YAML::Key << "value";
                l_yaml_emitter << YAML::Value << j->value;
                break;
            }

            l_yaml_emitter << YAML::EndMap;
        }
        l_yaml_emitter << YAML::EndSeq;

        try
        {
            l_fout << std::endl;
        }
        catch( ... )
        {
            std::cerr << "writing failed" << std::endl;
        }
    }

    const std::vector< JournalItem * > & getJournal() const
    {
        return m_journal;
    }

    void push_back( JournalItem *item )
    {
        m_journal.push_back( item );
    }

    void clear()
    {
        BOOST_FOREACH( JournalItem *i, m_journal )
        {
            delete i;
        }

        m_journal.clear();
    }

private:

    ChangeSet( const ChangeSet & );
    ChangeSet & operator=( const ChangeSet & );

    void load( const std::string &journalFileName )
    {
        assert( m_journal.empty() );
        if( ! boost::filesystem::exists( journalFileName ) )
        {
            return;
        }
        YAML::Node l_import = YAML::LoadFile( journalFileName );
        BOOST_FOREACH( YAML::Node n, l_import )
        {
            assert( n["update"] );
            assert( n["type"] );
            assert( n["time"] );
            std::string l_uid = n["update"].as< std::string >();
            std::string l_type = n["type"].as< std::string >();

            JournalItem *l_newItem;
            if( l_type == "CreateItem" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::CreateItem );
                assert( n["caption"] );
                l_newItem->value = n["caption"].as< std::string >();
            }
            if( l_type == "SetStringValue" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::SetStringValue );
                assert( n["name"] );
                assert( n["value"] );
                l_newItem->key = n["name"].as< std::string >();
                l_newItem->value = n["value"].as< std::string >();
            }
            if( l_type == "EraseItem" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::EraseItem );
            }
            if( l_type == "AddAttribute" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::AddAttribute );
                assert( n["name"] );
                l_newItem->key = n["name"].as< std::string >();
            }
            if( l_type == "RemoveAttribute" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::RemoveAttribute );
                assert( n["name"] );
                l_newItem->key = n["name"].as< std::string >();
            }
            if( l_type == "ChangeCaption" )
            {
                l_newItem = new JournalItem( l_uid, JournalItem::ChangeCaption );
                assert( n["value"] );
                l_newItem->value = n["value"].as< std::string >();
            }
            l_newItem->time = n["time"].as< std::string >();
            m_journal.push_back( l_newItem );
        }
    }

    std::vector< JournalItem * > m_journal;

};

}

#endif
