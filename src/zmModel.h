/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include "zmTrace.h"

#include <map>
#include <set>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/functional/hash.hpp>

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

class ThingsModel
{
public:
    class Thing
    {
    public:

        Thing( const std::string &caption)
            : m_caption   ( caption )
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

        static bool stringFind(const std::string &bigString, const std::string &pattern  )
        {
            std::string l_tmpBigString(bigString);

            std::transform(
                        l_tmpBigString.begin(),
                        l_tmpBigString.end(),
                        l_tmpBigString.begin(), ::tolower );

            return l_tmpBigString.find(pattern) != std::string::npos;
        }

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

        bool equals( const Thing & other )
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
            boost::hash<std::string> string_hash;
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


        std::string m_caption;
        std::set< std::string > m_attributes;

        typedef std::map< std::string, std::string > string_value_map_type;

        string_value_map_type m_string_values;
    };

    typedef std::map< std::string, Thing * > ThingsModelMapType;


    const ThingsModelMapType & things() const
    {
        return m_things;
    }


private:

    ThingsModelMapType m_things;
    ThingsModelMapType m_thingsOnLoad;


public:

    void load( const std::string &filename )
    {
        clear( m_things );
        clear( m_thingsOnLoad );

        if( ! boost::filesystem::exists( filename ) )
        {
            return;
        }
        YAML::Node l_import = YAML::LoadFile(filename);

        yamlToThingsMap( l_import, m_things );
        yamlToThingsMap( l_import, m_thingsOnLoad );
    }

    bool equals( const ThingsModelMapType &thingsMap, const ThingsModelMapType &thingsMapOther )
    {
        if( thingsMap.size() != thingsMapOther.size() )
        {
            return false;
        }
        for( ThingsModelMapType::const_iterator
             i  = thingsMap.begin();
             i != thingsMap.end(); ++i )
        {
            ThingsModelMapType::const_iterator l_mirrorThing = thingsMapOther.find( i->first );
            if( l_mirrorThing == thingsMapOther.end() )
            {
                return false;
            }
            if( !i->second->equals( *l_mirrorThing->second ) )
            {
                return false;
            }
        }
        return true;
    }

    void save( const std::string &filename )
    {
        if( filename.find_last_of("/") != std::string::npos )
        {
            std::string l_dir = filename.substr( 0, filename.find_last_of("/") );

            try
            {
                boost::filesystem::create_directory( l_dir );
            }
            catch( ... )
            {
                //error
            }

        }
        std::ofstream l_fout( filename.c_str() );

        assert( l_fout.is_open() );

        if( m_things.empty() )
        {
            return;
        }

        /// NOTE: we do the yaml exporting here semi manually
        ///       because this way we can ensure consistent
        ///       order of items between savings

        YAML::Emitter l_yaml_emitter( l_fout );

        l_yaml_emitter << YAML::BeginSeq;

        BOOST_FOREACH(const ThingsModelMapType::value_type& i, m_things)
        {
            l_yaml_emitter << YAML::VerbatimTag(i.first);
            l_yaml_emitter << YAML::BeginMap;

            l_yaml_emitter << YAML::Key << "caption";
            l_yaml_emitter << YAML::Value << i.second->m_caption;

            l_yaml_emitter << YAML::Key << "hash1";
            l_yaml_emitter << YAML::Value << i.second->getHash();

            if( ! i.second->m_attributes.empty() )
            {
                l_yaml_emitter << YAML::Key << "attributes";
                l_yaml_emitter << YAML::Value;
                l_yaml_emitter << YAML::BeginSeq;
                std::vector< std::string > v;
                BOOST_FOREACH( const std::string &a, i.second->m_attributes)
                {
                    l_yaml_emitter << a;
                }
                l_yaml_emitter << YAML::EndSeq;
            }

            if( ! i.second->m_string_values.empty() )
            {
                l_yaml_emitter << YAML::Key << "string_values";
                l_yaml_emitter << YAML::Value;
                /// this list is sorted anyway
                l_yaml_emitter << i.second->m_string_values;
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

    size_t getItemCount() const
    {
        return m_things.size();
    }

    std::string createNewItem( const std::string &caption )
    {
        std::string l_new_key = generateUid();
        Thing *l_new_thing = new Thing( caption );
        l_new_thing->addValue( "global_time_created", time_stamp() );

        m_things[ l_new_key ] = l_new_thing;
        return l_new_key;
    }

    bool hasItem( const std::string &uid ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        return l_item_it != m_things.end();
    }

    void eraseItem( const std::string &uid )
    {
        ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        m_things.erase( l_item_it );
    }

    void addAttribute( const std::string &uid, const std::string &attribute )
    {
        ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        l_item_it->second->addAttribute( attribute );
    }

    bool removeAttribute( const std::string &uid, const std::string &attribute )
    {
        ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->removeAttribute( attribute );
    }

    void setValue( const std::string &uid, const std::string &name, const std::string &value )
    {
        ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        l_item_it->second->addValue( name, value );
    }

    std::string getValue( const std::string &uid, const std::string &name ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->getValue( name );
    }

    void setCaption( const std::string &uid, const std::string &caption )
    {
        ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        l_item_it->second->m_caption = caption;
    }

    const std::string & getCaption( const std::string &uid ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->m_caption;
    }

    bool hasAttribute( const std::string uid, const std::string attribute ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->hasAttribute( attribute );
    }

    bool hasValue( const std::string uid, const std::string name ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->hasValue( name );
    }

    bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const
    {
        ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->contentMatchesString( searchString );
    }

    static std::string time_stamp()
    {
        return boost::posix_time::to_iso_extended_string(
                    boost::posix_time::microsec_clock::local_time());
    }

private:

    // returns 16x8 bit
    static inline std::string generateUid()
    {
        std::string l_return;
        l_return.reserve(16);
        l_return.resize(16);
        for( int i = 0; i < 16; ++i )
        {
            const char *l_characters = "0123456789abcdef";
            l_return[i] = l_characters[ rand() % 16 ];
        }
        return l_return;
    }

    static void clear( ThingsModelMapType &thingsMap )
    {
        BOOST_FOREACH(const ThingsModelMapType::value_type& i, thingsMap)
        {
            delete i.second;
        }
        thingsMap.clear();
    }

    static void yamlToThingsMap( YAML::Node yamlNode, ThingsModelMapType &thingsMap )
    {
        BOOST_FOREACH( YAML::Node n, yamlNode )
        {
            assert( n["caption"] );

            std::string l_uid =     n.Tag();
            std::string l_caption = n["caption"].as< std::string >();

            tracemessage("caption: '%s'", l_caption.c_str());

            Thing *l_new_thing = new Thing( l_caption );
            if( n["attributes"] )
            {
                std::vector< std::string > l_attributes =
                        n["attributes"].as< std::vector< std::string > >();
                for( std::vector< std::string >::const_iterator
                     a  = l_attributes.begin();
                     a != l_attributes.end(); ++ a )
                {
                    l_new_thing->m_attributes.insert( *a );
                }
            }
            if( n["string_values"] )
            {
                l_new_thing->m_string_values =
                    n["string_values"].as< Thing::string_value_map_type >();

                BOOST_FOREACH(
                    const Thing::string_value_map_type::value_type &a,
                    l_new_thing->m_string_values )
                {
                    std::cout << a.first << ": " << a.second << std::endl;
                }
            }

            if( n["hash1"] )
            {
                assert( n["hash1"].as< std::string >() == l_new_thing->getHash() );
            }
            assert( l_new_thing->hasValue("global_time_created") );

            thingsMap[ l_uid ] = l_new_thing;
        }
    }
};

#endif
