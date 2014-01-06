/// -*- coding: utf-8 -*-
///
/// file: zmOptions.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#pragma once
#ifndef ZMOPTIONS_H
#define ZMOPTIONS_H

#include <mm/zmCommon.h>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

namespace boost_ptree = boost::property_tree;

namespace zm
{

class zmOptions
{
    boost_ptree::ptree  m_tree;
    std::string         m_filename;
    bool                m_autosave;

public:

    zmOptions()
        : m_tree    ()
        , m_filename( "zm_config-fallback.json" )
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

        if( !boost::filesystem::exists( m_filename ) )
        {
            return;
        }

        try
        {
            boost_ptree::json_parser::read_json( m_filename, m_tree );
        }
        catch( ... )
        {
            // [todo] - handle
        }
    }

    bool hasValue( const std::string &key ) const
    {
        return m_tree.count( key ) > 0;
    }

    std::string getString( const std::string &key ) const
    {
        return m_tree.get< std::string >( key );
    }

    long getInt( const std::string &key ) const
    {
        return m_tree.get< long >( key );
    }

    std::vector< std::string > getStringList( const std::string &key ) const
    {
        boost::optional< const boost_ptree::ptree& > l_existing_child
                ( m_tree.get_child_optional( key ) );

        if( !l_existing_child )
        {
            return std::vector< std::string >();
        }

        std::vector< std::string > l_return;
        l_return.reserve( l_existing_child.get().size() );

        BOOST_FOREACH( const boost_ptree::ptree::value_type &v, l_existing_child.get())
        {
            l_return.push_back( v.second.data() );
        }

        return l_return;
    }

    void addStringListElement( const std::string &key, const std::string &value )
    {
        boost::optional< boost_ptree::ptree& > l_existing_child
                ( m_tree.get_child_optional( key ) );

        if( l_existing_child )
        {
            l_existing_child.get().push_back(std::make_pair( "", value));
        }
        else
        {
            boost_ptree::ptree l_tmp;
            l_tmp.push_back( std::make_pair( "", value ) );
            m_tree.put_child( key,l_tmp);
        }

        if( m_autosave ) save( m_filename );
    }

    void setString( const std::string &key, const std::string &value )
    {
        m_tree.put( key, value );

        if( m_autosave ) save( m_filename );
    }

    void setInt( const std::string &key, long value )
    {
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

}

#endif
