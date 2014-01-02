/// -*- coding: utf-8 -*-
///
/// file: zmChangeSet.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/mm/zmChangeSet.h"

//#include "zmTrace.h"
//#include "osal.h"
#include "include/mm/zmCommon.h"

#include <fstream>
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

bool zm::ChangeSet::write( const std::string &journalFileName )
{
    if( m_journal.size() == 0 ) return false;

    std::ofstream l_fout( journalFileName.c_str() );

    assert( l_fout.is_open() );

    /// NOTE: we do the yaml exporting here semi manually
    ///       because this way we can ensure consistent
    ///       order of items between savings

    YAML::Emitter l_yaml_emitter( l_fout );

    l_yaml_emitter << YAML::BeginSeq;

    BOOST_FOREACH(const journal_ptr_t &j, m_journal)
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
            l_yaml_emitter << YAML::Key << "value";
            l_yaml_emitter << YAML::Value << j->value;
            break;
        case JournalItem::Disconnect:
            l_yaml_emitter << YAML::Value << "Disonnect";
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
        std::cerr << "writing failed" << std::endl;
    }
    return true;
}

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
    BOOST_FOREACH( YAML::Node n, l_import )
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
            l_newItem = JournalItem::createConnect(
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
            assert( false && "type is not being handled" );
        }
        l_newItem->time = str(n["time"]);
        m_journal.push_back( l_newItem );
    }
}

void zm::ChangeSet::clear()
{   /*
    BOOST_FOREACH( JournalItem *i, m_journal )
    {
        delete i;
    }
    */

    m_journal.clear();
}

void  zm::ChangeSet::append(journal_item_vec_t a_changes)
{
    /// inserts a_changes into m_journal like
    /// m_journal.insert(m_journal.end(), changes.begin(), changes.end());
    /// but ensures that CreateItem events are located at the beginning

    BOOST_FOREACH(journal_item_vec_t::value_type &l_change, a_changes)
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
