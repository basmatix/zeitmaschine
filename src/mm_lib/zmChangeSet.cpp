/// -*- coding: utf-8 -*-
///
/// file: zmChangeSet.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/mm/zmChangeSet.h"

#include "include/mm/zmTrace.h"
//#include "osal.h"
#include "include/mm/zmCommon.h"

#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

#include <boost/filesystem.hpp>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

bool zm::ChangeSet::write( const std::string &journalFileName )
{
    if( m_journal.size() == 0 )
    {
        return false;
    }

    zm::common::create_base_directory(journalFileName);

    std::ofstream l_fout( journalFileName.c_str() );

    assert( l_fout.is_open() );

    /// NOTE: we do the yaml exporting here semi manually
    ///       because this way we can ensure consistent
    ///       order of items between savings

    YAML::Emitter l_yaml_emitter( l_fout );

    l_yaml_emitter << YAML::BeginSeq;

    for(const journal_ptr_t &j: m_journal)
    {
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "update";
        l_yaml_emitter << YAML::Value << j->item_uid;

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
        case JournalItem::ChangeCaption:
            l_yaml_emitter << YAML::Value << "ChangeCaption";
            l_yaml_emitter << YAML::Key << "value";
            l_yaml_emitter << YAML::Value << j->value;
            break;
        case JournalItem::Connect:
            l_yaml_emitter << YAML::Value << "Connect";
            l_yaml_emitter << YAML::Key << "key";
            l_yaml_emitter << YAML::Value << j->key;
            l_yaml_emitter << YAML::Key << "value";
            l_yaml_emitter << YAML::Value << j->value;
            break;
        case JournalItem::Disconnect:
            l_yaml_emitter << YAML::Value << "Disconnect";
            l_yaml_emitter << YAML::Key << "value";
            l_yaml_emitter << YAML::Value << j->value;
            break;
        case JournalItem::AddAttribute:
        default:
            assert(false && "jounal type must be valid");
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
        trace_e("writing failed");
    }
    return true;
}

/// just for convenience
inline std::string str(const YAML::Node &n)
{
    return n.as< std::string >();
}

void zm::ChangeSet::load( const std::string &journalFileName )
{
    assert( m_journal.empty() );
    if( ! boost::filesystem::exists( journalFileName ) )
    {
        return;
    }
    YAML::Node l_import = YAML::LoadFile( journalFileName );
    for( YAML::Node n: l_import )
    {
        assert( n["update"] );
        assert( n["type"] );
        assert( n["time"] );
        std::string l_uid = str(n["update"]);
        std::string l_type = str(n["type"]);

        journal_ptr_t l_newItem;
        if( l_type == "CreateItem" )
        {
            assert( n["caption"] );
            l_newItem = JournalItem::createCreate(
                        l_uid, str(n["caption"]) );
        }
        else if( l_type == "EraseItem" )
        {
            l_newItem = JournalItem::createErase(l_uid);
        }
        else if( l_type == "ChangeCaption" )
        {
            assert( n["value"] );
            l_newItem = JournalItem::createChangeCaption(l_uid,
                        str(n["value"]));
        }
        else if( l_type == "SetStringValue" )
        {
            assert( n["name"] );
            assert( n["value"] );
            l_newItem = JournalItem::createSetStringValue(
                        l_uid,
                        str(n["name"]),
                        str(n["value"]));
        }
        else if( l_type == "Connect" )
        {
            assert( n["value"] );
            if(n["key"])
            {
                l_newItem = JournalItem::createConnect(
                            l_uid,
                            zm::neighbour_t(str(n["key"]), n["value"].as<int>()));
            }
            else
            {
                // old deprecated way: only uid is given as value
                l_newItem = JournalItem::createConnect(
                            l_uid,
                            zm::neighbour_t(str(n["value"]), 1));
            }
        }
        else if( l_type == "Disconnect" )
        {
            assert( n["value"] );
            l_newItem = JournalItem::createDisconnect(
                        l_uid,
                        str(n["value"]));
        }
        else if( l_type == "AddAttribute" )
        {
            assert( n["name"] );
            l_newItem = JournalItem::createAddAttribute(
                        l_uid,
                        str(n["name"]));
        }
        else
        {
            trace_i("type = '%s'", l_type.c_str());
            assert( false && "type is not being handled" );
        }
        l_newItem->time = str(n["time"]);
        m_journal.push_back( l_newItem );
    }
}

void zm::ChangeSet::clear()
{
    m_journal.clear();
}

void zm::ChangeSet::append(journal_item_vec_t a_changes)
{
    /// inserts a_changes into m_journal like
    /// m_journal.insert(m_journal.end(), changes.begin(), changes.end());
    /// but ensures that CreateItem events are located at the beginning

    for(journal_item_vec_t::value_type &l_change: a_changes)
    {
        if( l_change->type == JournalItem::CreateItem )
        {
            m_journal.push_front(l_change);
        }
        else
        {
            m_journal.push_back(l_change);
        }
    }
}

void zm::ChangeSet::debug_dump() const
{
    for( const journal_item_vec_t::value_type &l_change: m_journal)
    {
        trace_i("  on %s: (t:%s, k:%s, v:%s)",
                     l_change->item_uid.c_str(),
                     l_change->type == JournalItem::CreateItem     ? "CreateItem    " :
                     l_change->type == JournalItem::EraseItem      ? "EraseItem     " :
                     l_change->type == JournalItem::SetStringValue ? "SetStringValue" :
                     l_change->type == JournalItem::AddAttribute   ? "AddAttribute  " :
                     l_change->type == JournalItem::ChangeCaption  ? "ChangeCaption " :
                     l_change->type == JournalItem::Connect        ? "Connect       " :
                     l_change->type == JournalItem::Disconnect     ? "Disconnect    " :
                                                                     "invalid",
                     l_change->key.c_str(),
                     l_change->value.c_str());
    }
}
