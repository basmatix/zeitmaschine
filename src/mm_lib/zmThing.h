/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef THING_H
#define THING_H

#include "mm/zmChangeSet.h"

#include <set>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include <algorithm>
#include <boost/functional/hash.hpp>

#include <assert.h>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

namespace zm
{
    class MindMatter
    {

    public:

        typedef std::map< std::string, std::string > string_value_map_type;

        std::string                 m_caption;    // todo: should be value
        string_value_map_type       m_string_values;
        std::set< MindMatter * >    m_neighbours;

    public:

        MindMatter( const std::string &caption)
            : m_caption         ( caption )
            , m_string_values   ()
            , m_neighbours      ()
        {
        }

        void addValue( const std::string &name, const std::string &value )
        {
            m_string_values[ name ] = value;
        }

        bool hasValue( const std::string &name ) const
        {
            return m_string_values.find( name ) != m_string_values.end();
        }

        std::string getValue( const std::string &name ) const
        {
            string_value_map_type::const_iterator i
                    = m_string_values.find( name );
            assert( i != m_string_values.end() );

            return i->second;
        }

        // todo: should be visitor stuff
        static bool stringFind(const std::string &bigString, const std::string &pattern  )
        {
            std::string l_tmpBigString(bigString);

            std::transform(
                        l_tmpBigString.begin(),
                        l_tmpBigString.end(),
                        l_tmpBigString.begin(), ::tolower );

            return l_tmpBigString.find(pattern) != std::string::npos;
        }

        // todo: should be visitor stuff
        bool contentMatchesString( const std::string &searchString ) const
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

        bool equals( const MindMatter & other )
        {
            if( m_caption != other.m_caption )
            {
                return false;
            }

            if(m_string_values.size() != other.m_string_values.size()
            || m_neighbours.size()    != other.m_neighbours.size() )
            {
                return false;
            }

            //TODO: check string_values

            //TODO: check neighbours
            return true;
        }

        /// returns a journal creating this item
        journal_item_vec_t toDiff( const std::string &uid ) const
        {
            journal_item_vec_t l_result;
            l_result.push_back(JournalItem::createCreate(uid, m_caption));

            // [todo] - change caption
            // [todo] - change neighbours
            // [todo] - change string values
            return l_result;
        }

        /// returns a journal which would turn this item into the other
        journal_item_vec_t diff( const MindMatter & other ) const
        {
            journal_item_vec_t l_result;
            // [todo] - check caption
            // [todo] - check neighbours
            // [todo] - check string values
            return l_result;
        }

        std::string getHash( ) const
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

        bool operator== ( const MindMatter &other )
        {
            return false;
        }

        bool operator!= ( const MindMatter &other )
        {
            return !this->operator ==( other );
        }
    };
}
#endif
