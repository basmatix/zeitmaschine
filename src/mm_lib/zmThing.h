/// -*- coding: utf-8 -*-
/// copyright (C) 2013 Frans Fuerst

#ifndef THING_H
#define THING_H

#include "mm/zmChangeSet.h"

#include <set>
#include <string>
#include <map>

namespace zm
{
    class MindMatter
    {

    public:

        typedef std::map< std::string, std::string >   string_value_map_type;
        typedef std::pair< MindMatter *, std::string > item_uid_pair_t;
        typedef std::map< MindMatter *, std::string >  item_uid_map_t;

        std::string                 m_caption;    // [todo] - should be value
        string_value_map_type       m_string_values;
        //std::set< MindMatter * >    m_neighbours;
        item_uid_map_t              m_neighbours_;

    public:

        MindMatter( const std::string &caption);

        void addValue( const std::string &name, const std::string &value );

        bool hasValue( const std::string &name ) const;

        std::string getValue( const std::string &name ) const;

        // [todo] - should be visitor stuff
        static bool stringFind(const std::string &bigString, const std::string &pattern);

        // todo: should be visitor stuff
        bool contentMatchesString( const std::string &searchString ) const;

        bool equals( const MindMatter & other );

        /// returns a journal creating this item
        journal_item_vec_t toDiff( const std::string &uid ) const;

        /// returns a journal which would turn this item into the other
        journal_item_vec_t diff(
                const std::string &uid,
                const MindMatter  &other ) const;

        std::string getHash( ) const;

        bool operator== ( const MindMatter &other );

        bool operator!= ( const MindMatter &other );
    };
}

#include <fstream>
#include <sstream>

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>

#include <assert.h>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings


zm::MindMatter::MindMatter( const std::string &caption)
    : m_caption         ( caption )
    , m_string_values   ()
    , m_neighbours_     ()
{
}

void zm::MindMatter::addValue( const std::string &name, const std::string &value )
{
    m_string_values[ name ] = value;
}

bool zm::MindMatter::hasValue( const std::string &name ) const
{
    return m_string_values.find( name ) != m_string_values.end();
}

std::string zm::MindMatter::getValue( const std::string &name ) const
{
    string_value_map_type::const_iterator i
            = m_string_values.find( name );
    assert( i != m_string_values.end() );

    return i->second;
}

// todo: should be visitor stuff
bool zm::MindMatter::stringFind(const std::string &bigString, const std::string &pattern  )
{
    std::string l_tmpBigString(bigString);

    std::transform(
                l_tmpBigString.begin(),
                l_tmpBigString.end(),
                l_tmpBigString.begin(), ::tolower );

    return l_tmpBigString.find(pattern) != std::string::npos;
}

// todo: should be visitor stuff
bool zm::MindMatter::contentMatchesString( const std::string &searchString ) const
{
    if( stringFind( m_caption, searchString ) )
    {
        return true;
    }
    for( string_value_map_type::const_iterator
         i  = m_string_values.begin();
         i != m_string_values.end(); ++i )
    {
        if( stringFind( i->second, searchString ) )
        {
            return true;
        }
    }
    return false;
}

bool zm::MindMatter::equals( const MindMatter & other )
{
    if( m_caption != other.m_caption )
    {
        return false;
    }

    if(m_string_values.size() != other.m_string_values.size()
    || m_neighbours_.size()    != other.m_neighbours_.size() )
    {
        return false;
    }

    //TODO: check string_values

    //TODO: check neighbours
    return true;
}

zm::journal_item_vec_t zm::MindMatter::toDiff( const std::string &a_uid ) const
{
    journal_item_vec_t l_result;

    l_result.push_back(JournalItem::createCreate(a_uid, m_caption));

    BOOST_FOREACH(const string_value_map_type::value_type &l_entry, m_string_values)
    {
        l_result.push_back(JournalItem::createSetStringValue(
                               a_uid, l_entry.first, l_entry.second));
    }

    BOOST_FOREACH(const item_uid_pair_t l_neighbour, m_neighbours_)
    {
        l_result.push_back(JournalItem::createConnect(
                               a_uid, l_neighbour.second));
    }

    return l_result;
}


std::set< std::string > get_neighbour_uids(
        const zm::MindMatter::item_uid_map_t &neighbours )
{
    std::set< std::string > l_result;

    BOOST_FOREACH(const zm::MindMatter::item_uid_pair_t l_neighbour, neighbours)
    {
        std::pair< std::set< std::string >::iterator, bool > l_success =
                l_result.insert(l_neighbour.second);

        assert( l_success.second );
    }
    return l_result;
}

zm::journal_item_vec_t zm::MindMatter::diff(
        const std::string &a_uid,
        const MindMatter  &a_other ) const
{
    journal_item_vec_t l_result;

    ///
    /// compare caption
    ///

    if( m_caption != a_other.m_caption)
    {
        l_result.push_back(JournalItem::createChangeCaption(
                               a_uid, a_other.m_caption));
    }

    ///
    /// compare string values
    ///

    std::set< std::string > l_done_keys;

    /// go through all elements of m_string_values
    BOOST_FOREACH( const string_value_map_type::value_type &i, m_string_values )
    {
        const std::string &l_key(i.first);

        l_done_keys.insert( l_key );

        /// find the key in the other map
        string_value_map_type::const_iterator l_other_it(
                    a_other.m_string_values.find( l_key ) );

        if( l_other_it == a_other.m_string_values.end() )
        {
            l_result.push_back(JournalItem::createSetStringValue(
                                   a_uid, l_key, ""));
            continue;
        }
        if( i.second != l_other_it->second )
        {
            l_result.push_back(JournalItem::createSetStringValue(
                                   a_uid, l_key, l_other_it->second));
        }
    }

    BOOST_FOREACH( const string_value_map_type::value_type &i, a_other.m_string_values )
    {
        const std::string &l_other_key(i.first);

        /// prune keys we handled already
        if( l_done_keys.find( l_other_key ) != l_done_keys.end() )
        {
            continue;
        }

        l_result.push_back(JournalItem::createSetStringValue(
                               a_uid, l_other_key, i.second));
    }

    ///
    /// compare connections
    ///

    std::set< std::string > l_this_neighbours(
                get_neighbour_uids(m_neighbours_));

    std::set< std::string > l_other_neighbours(
            get_neighbour_uids(a_other.m_neighbours_));

    std::set< std::string > l_only_in_this;

    std::set_difference(
                l_this_neighbours.begin(), l_this_neighbours.end(),
                l_other_neighbours.begin(), l_other_neighbours.end(),
                std::inserter(
                    l_only_in_this, l_only_in_this.end()));

    std::set< std::string > l_only_in_other;

    std::set_difference(
                l_other_neighbours.begin(), l_other_neighbours.end(),
                l_this_neighbours.begin(), l_this_neighbours.end(),
                std::inserter(
                    l_only_in_other, l_only_in_other.end()));

    BOOST_FOREACH( const std::string &i, l_only_in_this )
    {
        l_result.push_back(JournalItem::createDisconnect(
                               a_uid, i));
    }

    BOOST_FOREACH( const std::string &i, l_only_in_other )
    {
        l_result.push_back(JournalItem::createConnect(
                               a_uid, i));
    }

    return l_result;
}

std::string zm::MindMatter::getHash( ) const
{
    std::stringstream l_stream;
    boost::hash< std::string > string_hash;
    l_stream << string_hash( m_caption );

    /// important: enforce predictable order!

    /// maps should be sorted by key anyway..
    for( string_value_map_type::const_iterator
         i  = m_string_values.begin();
         i != m_string_values.end(); ++i )
    {
        l_stream << string_hash( i->second );
    }

    //TODO: handle neighbours

    return l_stream.str();
}

bool zm::MindMatter::operator== ( const MindMatter &other )
{
    return false;
}

bool zm::MindMatter::operator!= ( const MindMatter &other )
{
    return !this->operator ==( other );
}

#endif
