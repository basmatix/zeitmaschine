/// -*- coding: utf-8 -*-
/// copyright (C) 2013 Frans Fuerst

#ifndef THING_H
#define THING_H

#include "mm/zmChangeSet.h"
#include "mm/zmTypes.h"

#include <set>
#include <string>
#include <map>

namespace zm
{
    class MindMatter
    {
        /// MindMatter object cannot be copyied because the neighbour map
        /// map must be constructed from outside
        MindMatter(const MindMatter &);
        MindMatter & operator=(const MindMatter &);

    public:

        std::string                 m_caption;    // [todo] - should be value
        string_value_map_type       m_string_values;
        item_neighbour_map_t        m_neighbours;

    public:

        inline MindMatter( const std::string &caption);

        inline void addValue( const std::string &name, const std::string &value );

        inline bool hasValue( const std::string &name ) const;

        inline std::string getValue( const std::string &name ) const;

        // [todo] - should be visitor stuff
        inline static bool stringFind(const std::string &bigString, const std::string &pattern);

        // todo: should be visitor stuff
        inline bool contentMatchesString( const std::string &searchString ) const;

        inline bool equals( const MindMatter & other, bool tell_why = false );

        /// returns a journal creating this item
        inline journal_item_vec_t toJournal( const std::string &uid ) const;

        /// returns a journal which would turn this item into the other
        inline journal_item_vec_t diff(
                const std::string &uid,
                const MindMatter  &other ) const;

        inline std::set< std::string > getNeighbourUids() const;

        inline std::string createHash( bool verbose=false ) const;

        inline bool operator== ( const MindMatter &other );

        inline bool operator!= ( const MindMatter &other );
    };
}

#include <fstream>
#include <sstream>

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>

#include <mm/zmTrace.h>

#include <assert.h>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings


zm::MindMatter::MindMatter( const std::string &caption)
    : m_caption         ( caption )
    , m_string_values   ()
    , m_neighbours     ()
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

bool zm::MindMatter::equals( const MindMatter & other, bool tell_why )
{
    if( m_caption != other.m_caption )
    {
        if(tell_why)
        {
            tracemessage( "equals(): captions differ. '%s'<>'%s'",
                          m_caption.c_str(), other.m_caption.c_str() );
        }
        return false;
    }

    if(m_string_values.size() != other.m_string_values.size() )
    {
        if(tell_why)
        {
            tracemessage( "equals(): sizes of string values differ." );
        }
        return false;
    }

    if(m_neighbours.size() != other.m_neighbours.size() )
    {
        if(tell_why)
        {
            tracemessage( "equals(): sizes of m_neighbours differ." );
        }
        return false;
    }

    if( m_string_values != other.m_string_values )
    {
        if(tell_why)
        {
            tracemessage( "equals(): string values differ" );
        }
        return false;
    }

    if( getNeighbourUids() != other.getNeighbourUids() )
    {
        if(tell_why)
        {
            tracemessage( "equals(): connected neighbours differ" );
        }
        return false;
    }

    return true;
}

zm::journal_item_vec_t zm::MindMatter::toJournal( const std::string &a_uid ) const
{
    journal_item_vec_t l_result;

    l_result.push_back(JournalItem::createCreate(a_uid, m_caption));

    for(const string_value_map_type::value_type &l_entry: m_string_values)
    {
        l_result.push_back(JournalItem::createSetStringValue(
                               a_uid, l_entry.first, l_entry.second));
    }

    for(const item_neighbour_pair_t l_neighbour: m_neighbours)
    {
        l_result.push_back(JournalItem::createConnect(
                               a_uid, l_neighbour.second));
    }

    return l_result;
}

std::set< std::string > zm::MindMatter::getNeighbourUids() const
{
    std::set< std::string > l_result;

    for(const zm::item_neighbour_pair_t l_neighbour:
                  m_neighbours)
    {
        std::pair< std::set< std::string >::iterator, bool > l_success =
                l_result.insert(l_neighbour.second.first);

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
    for( const string_value_map_type::value_type &i: m_string_values )
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

    for( const string_value_map_type::value_type &i: a_other.m_string_values )
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
                getNeighbourUids());

    std::set< std::string > l_other_neighbours(
                a_other.getNeighbourUids());

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

    for( const std::string &i: l_only_in_this )
    {
        l_result.push_back(JournalItem::createDisconnect(
                               a_uid, i));
    }

    for( const std::string &i: l_only_in_other )
    {
        l_result.push_back(JournalItem::createConnect(
                               a_uid, i));
    }

    return l_result;
}

inline void hash_add_value(
        size_t             &hash,
        const std::string  &value,
        bool                verbose )
{
    if( verbose )
    {
        tracemessage("adding '%s' to hash", value.c_str());
    }
    boost::hash_combine(hash, value);
}

std::string zm::MindMatter::createHash( bool a_verbose ) const
{
    size_t l_hash(0);

    hash_add_value(l_hash, m_caption, a_verbose);

    /// IMPORTANT: enforce predictable order!

    /// maps are sorted by key
    BOOST_FOREACH( const string_value_map_type::value_type &l_value,
                   m_string_values)
    {
        hash_add_value(l_hash, l_value.second, a_verbose);
    }

    /// sets are sorted, too
    BOOST_FOREACH( const std::string &l_uid,
                   getNeighbourUids())
    {
        hash_add_value(l_hash, l_uid, a_verbose);
    }

    std::stringstream l_result_stream;
    l_result_stream << std::hex << l_hash;

    return l_result_stream.str();
}

bool zm::MindMatter::operator== ( const MindMatter &other )
{
    return equals(other);
}

bool zm::MindMatter::operator!= ( const MindMatter &other )
{
    return !equals(other);
}

#endif
