/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef THING_H
#define THING_H


#include <set>
#include <fstream>
#include <sstream>
#include <string>

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
        std::set< std::string >     m_attributes;
        string_value_map_type       m_string_values;
        std::set< MindMatter * >    m_neighbours;

    public:

        MindMatter( const std::string &caption)
            : m_caption         ( caption )
            , m_attributes      ()
            , m_string_values   ()
            , m_neighbours      ()
        {
        }

        void addAttribute( const std::string &attribute )
        {
            m_attributes.insert( attribute );
        }

        bool removeAttribute( const std::string &attribute )
        {
            if( m_attributes.find( attribute ) == m_attributes.end() )
            {
                return false;
            }
            else
            {
                m_attributes.erase( attribute );
                return true;
            }
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

        bool hasAttribute( const std::string &attribute ) const
        {
            return m_attributes.find( attribute ) != m_attributes.end();
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
            if( m_attributes != other.m_attributes )
            {
                return false;
            }
            return true;
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

            /// and fortunately this is true for sets, too
            for( std::set< std::string >::const_iterator
                 i  = m_attributes.begin();
                 i != m_attributes.end(); ++i )
            {
                l_stream << string_hash( *i );
            }
            return l_stream.str();
        }
    };

}
#endif
