/// -*- coding: utf-8 -*-
///
/// file: zmOptions.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <mm/zmCommon.h>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace boost_ptree = boost::property_tree;

class zmOptions
{
    boost_ptree::ptree  m_tree;
    std::string         m_filename;
    bool                m_loaded;
    bool                m_autosave;

public:

    zmOptions()
        : m_tree    ()
        , m_filename( "zm_config-fallback.json" )
        , m_loaded  ( false )
        , m_autosave( true )
    {
    }

    void setAutosave( bool value )
    {
        m_autosave = value;
    }

    void load( const std::string &filename )
    {
        m_filename = filename;

        m_loaded = true;

        if( !boost::filesystem::exists( m_filename ) )
        {
            return;
        }

        boost_ptree::json_parser::read_json( m_filename, m_tree );
    }

    bool hasValue( const std::string &key ) const
    {
        assert( m_loaded );

        return m_tree.count( key ) > 0;
    }

    std::string getStringValue( const std::string &key ) const
    {
        assert( m_loaded );
        return m_tree.get< std::string >( key );
    }

    std::vector< std::string > getStringList( const std::string &key ) const
    {
        assert( m_loaded );

        if( m_tree.count( key ) == 0 )
        {
            return std::vector< std::string >();
        }

        const boost_ptree::ptree &l_tmp = m_tree.get_child( key );
        std::vector< std::string > l_return;
        l_return.reserve( l_tmp.size() );

        BOOST_FOREACH( const boost_ptree::ptree::value_type &v, l_tmp)
        {
            l_return.push_back( v.second.data() );
        }

        return l_return;
    }

    void addString( const std::string &key, const std::string &value )
    {
        assert( m_loaded );

        if( m_tree.count( key ) > 0 )
        {
            boost_ptree::ptree &l_tmp = m_tree.get_child( key );
            l_tmp.push_back(std::make_pair("", value));
        }
        else
        {
            boost_ptree::ptree l_tmp;
            l_tmp.push_back( std::make_pair("", value ) );
            m_tree.put_child( key,l_tmp);
        }

        if( m_autosave ) save( m_filename );
    }

    void setStringValue( const std::string &key, const std::string &value )
    {
        assert( m_loaded );

        m_tree.put( key, value );

        if( m_autosave ) save( m_filename );
    }

private:

    void save( const std::string &filename )
    {
        if( !zm::common::create_base_directory( filename ) )
        {
            // todo: error
            assert( !"zm::common::create_base_directory( filename )" );
            return;
        }

        std::ofstream l_fout( filename.c_str() );

        assert( l_fout.is_open() );

        boost::property_tree::json_parser::write_json( m_filename, m_tree );
    }
};

