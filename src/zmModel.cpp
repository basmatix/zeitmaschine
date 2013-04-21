/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#include "zmModel.h"

#include "zmTrace.h"
#include "zmOsal.h"

#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

class Options
{
public:

    Options()
        : m_variable_map()
        , m_descriptions("Generic options")
    {
        m_descriptions.add_options()
            ("username", "unique username - used for syncing")
                ("read_journal",  po::value< std::vector<std::string> >(),//->default_value( std::vector< std::string >() ),
                    "name of a journal file already parsed")
            ("hostname", "unique name for this machine - used for syncing")
            ;
    }

    void load( const std::string &filename )
    {
        //store(parse_command_line(argc, argv, desc), m_variable_map);

        boost::program_options::store(
                    boost::program_options::parse_config_file< char >(
                        filename.c_str(), m_descriptions ),
                    m_variable_map);

        boost::program_options::notify( m_variable_map );
    }

    bool hasValue( const std::string &key ) const
    {
        return m_variable_map.count( key ) > 0;
    }

    std::string getStringValue( const std::string &key ) const
    {
        return m_variable_map[ key ].as< std::string >();
    }

    std::vector< std::string > getStringList( const std::string &key ) const
    {
        return m_variable_map[ key ].as< std::vector< std::string > >();
    }

    void addString( const std::string &key, const std::string &value )
    {
        std::vector< std::string > l_tmp;

        bool l_value_exists = m_variable_map.count( key ) > 0;

        if( l_value_exists )
        {
            l_tmp = m_variable_map[ key ].as< std::vector< std::string > >();
        }

        l_tmp.push_back( value );

        /// insert does not override values - we have to trick boost
        if( l_value_exists )
        {
            boost::program_options::variables_map::iterator i = m_variable_map.find( key );
            i->second = boost::program_options::variable_value( l_tmp, false);
        }
        else
        {
            m_variable_map.insert( std::make_pair(
                key, boost::program_options::variable_value( l_tmp, false) ) );
        }

        boost::program_options::notify( m_variable_map );

        save( "zeitmaschine.cfg" );
    }

    void setStringValue( const std::string &key, const std::string &value )
    {
        boost::program_options::variables_map::iterator i = m_variable_map.find( key );
        if( i == m_variable_map.end() )
        {
            m_variable_map.insert( std::make_pair(
                key, boost::program_options::variable_value( value, false)) );
        }
        else
        {
            i->second = boost::program_options::variable_value( value, false);
        }

        boost::program_options::notify( m_variable_map );

        save( "zeitmaschine.cfg" );
    }

private:

    void save( const std::string &filename )
    {
        std::ofstream l_fout( filename.c_str() );

        BOOST_FOREACH( po::variables_map::value_type v, m_variable_map)
        {
            if ( v.second.empty() ) continue;
            const ::std::type_info &type = v.second.value().type() ;
            if ( type == typeid( ::std::string ) )
            {
                l_fout << v.first << " = " << v.second.as< std::string >() << std::endl;
            }
            else if ( type == typeid( std::vector< std::string > ) )
            {
                std::vector< std::string > l_tmp( v.second.as< std::vector< std::string > >() );
                tracemessage( "%d", l_tmp.size() );
                BOOST_FOREACH( std::string s, l_tmp)
                {
                    l_fout << v.first << " = " << s << std::endl;
                }
            }
        }
    }

    boost::program_options::variables_map       m_variable_map;
    boost::program_options::options_description m_descriptions;

// not gonna stay here
} m_options;

zm::ThingsModel::ThingsModel()
    : m_things              ()
    , m_thingsOnLoad        ()
    , m_localFolder         ( "" )
    , m_filename            ( "" )
    , m_temporaryJournalFile( "" )
    , m_initialized         ( false )
    , m_dirty               ( false )
    , m_changeSet           ()
{
    m_options.load("zeitmaschine.cfg");
}

void zm::ThingsModel::setLocalFolder( const std::string &path )
{
    tracemessage( "privateDir: %s", path.c_str() );
    m_localFolder = path;
}

void zm::ThingsModel::addDomainSyncFolder( const std::string &domainName, const std::string &path )
{
    tracemessage( "new domain: %s %s", domainName.c_str(), path.c_str() );
}

bool zm::ThingsModel::hasUsedUsername() const
{
    return m_options.hasValue( "username" )
            && m_options.getStringValue( "username" ) != "";
}

bool zm::ThingsModel::hasUsedHostname() const
{
    return m_options.hasValue( "hostname" )
            && m_options.getStringValue( "hostname" ) != "";
}

void zm::ThingsModel::setUsedUsername( const std::string &username )
{
    m_options.setStringValue( "username", username );
}

void zm::ThingsModel::setUsedHostname( const std::string &hostname )
{
    m_options.setStringValue( "hostname", hostname );
}

void zm::ThingsModel::initialize()
{
    /// find the name for the local model file - should be equal
    /// across sessions and unique for each client
    // <local folder>/zm-<user>-<client>-<zm-domain>-local.yaml
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-private-local.yaml
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolder << "/zm-"
                 << m_options.getStringValue( "username" )
                 << "-"
                 << m_options.getStringValue( "hostname" )
                 << "-local.yaml";

    m_filename = l_ssFileName.str();

    /// find the name for the session journal. must be unique
    /// across sessions
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-temp-journal.yaml
    std::stringstream l_ssTempJournalFile;
    l_ssTempJournalFile << m_localFolder << "/zm-"
                        << m_options.getStringValue( "username" )
                        << "-"
                        << m_options.getStringValue( "hostname" ) << "-"
                        << zm::common::time_stamp_iso()
                        << "-temp-journal.yaml";
    m_temporaryJournalFile = l_ssTempJournalFile.str();

    tracemessage( "using temp file '%s'", m_temporaryJournalFile.c_str() );

    load( m_filename );

    m_initialized = true;
}

void zm::ThingsModel::localSave()
{
    /// just for debug purposes - later we will only write the journal
    save( m_filename );

    m_changeSet.write( m_temporaryJournalFile );
}

void zm::ThingsModel::sync()
{
    save( m_filename );
}

void zm::ThingsModel::load( const std::string &filename )
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

void zm::ThingsModel::dirty()
{
    m_dirty = true;
}

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

void zm::ThingsModel::save( const std::string &filename )
{
    /// be careful! if( !m_dirty ) return;

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
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "uid";
        l_yaml_emitter << YAML::Value << i.first;

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

const zm::ThingsModel::ThingsModelMapType & zm::ThingsModel::things() const
{
    return m_things;
}

bool zm::ThingsModel::equals( const ThingsModelMapType &thingsMap, const ThingsModelMapType &thingsMapOther ) const
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

size_t zm::ThingsModel::getItemCount() const
{
    return m_things.size();
}

std::time_t zm::ThingsModel::getCreationTime( const std::string &uid ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    std::string l_timeStr = l_item_it->second->getValue( "global_time_created" );

    // unfortunately in boost 1.50 iso_format_extended_specifier only specifies to "%Y-%m-%dT"
    //return seconds_from_epoch( l_timeStr, boost::posix_time::time_facet::iso_format_extended_specifier );
    return zm::common::seconds_from_epoch( l_timeStr, "%Y-%m-%dT%H:%M:%S" );
}

bool zm::ThingsModel::hasItem( const std::string &uid ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    return l_item_it != m_things.end();
}

std::string zm::ThingsModel::getValue( const std::string &uid, const std::string &name ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    return l_item_it->second->getValue( name );
}

const std::string & zm::ThingsModel::getCaption( const std::string &uid ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    return l_item_it->second->m_caption;
}

bool zm::ThingsModel::hasAttribute( const std::string uid, const std::string attribute ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    return l_item_it->second->hasAttribute( attribute );
}

bool zm::ThingsModel::hasValue( const std::string uid, const std::string name ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    return l_item_it->second->hasValue( name );
}

bool zm::ThingsModel::itemContentMatchesString( const std::string &uid, const std::string &searchString ) const
{
    ThingsModelMapType::const_iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    return l_item_it->second->contentMatchesString( searchString );
}



///
/// write relevant interface
///

std::string zm::ThingsModel::createNewItem( const std::string &caption )
{
    std::string l_new_key = generateUid();
    Thing *l_new_thing = new Thing( caption );
    std::string l_time = zm::common::time_stamp_iso_ext();
    l_new_thing->addValue( "global_time_created", l_time );

    m_things[ l_new_key ] = l_new_thing;

    JournalItem *l_change = new JournalItem( l_new_key, JournalItem::CreateItem );
    l_change->time = l_time;
    l_change->value = caption;
    m_changeSet.push_back( l_change );

    dirty();

    return l_new_key;
}

void zm::ThingsModel::eraseItem( const std::string &uid )
{
    ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    m_things.erase( l_item_it );

    JournalItem *l_change = new JournalItem( uid, JournalItem::EraseItem );
    m_changeSet.push_back( l_change );

    dirty();
}

void zm::ThingsModel::addAttribute( const std::string &uid, const std::string &attribute )
{
    ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    l_item_it->second->addAttribute( attribute );

    JournalItem *l_change = new JournalItem( uid, JournalItem::AddAttribute );
    l_change->key = attribute;
    m_changeSet.push_back( l_change );

    dirty();
}

bool zm::ThingsModel::removeAttribute( const std::string &uid, const std::string &attribute )
{
    ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    bool l_return = l_item_it->second->removeAttribute( attribute );

    JournalItem *l_change = new JournalItem( uid, JournalItem::RemoveAttribute );
    l_change->key = attribute;
    m_changeSet.push_back( l_change );

    dirty();

    return l_return;
}

void zm::ThingsModel::setValue( const std::string &uid, const std::string &name, const std::string &value )
{
    ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    JournalItem *l_change = new JournalItem( uid, JournalItem::SetStringValue );
    //l_change->before = l_item_it->second->m_caption;
    l_change->key = name;
    l_change->value = value;
    m_changeSet.push_back( l_change );

    l_item_it->second->addValue( name, value );

    dirty();
}

void zm::ThingsModel::setCaption( const std::string &uid, const std::string &caption )
{
    ThingsModelMapType::iterator l_item_it( m_things.find( uid ) );

    assert( l_item_it != m_things.end() );

    if( l_item_it->second->m_caption == caption ) return;

    JournalItem *l_change = new JournalItem( uid, JournalItem::ChangeCaption );
    //l_change->before = l_item_it->second->m_caption;
    l_change->value = caption;
    m_changeSet.push_back( l_change );

    l_item_it->second->m_caption = caption;

    dirty();
}

void zm::ThingsModel::clear( ThingsModelMapType &thingsMap )
{
    BOOST_FOREACH(const ThingsModelMapType::value_type& i, thingsMap)
    {
        delete i.second;
    }
    thingsMap.clear();
}

void zm::ThingsModel::yamlToThingsMap( YAML::Node yamlNode, ThingsModelMapType &thingsMap )
{
    BOOST_FOREACH( YAML::Node n, yamlNode )
    {
        assert( n["caption"] );

        std::string l_uid = n.Tag();

        if( n["uid"] )
        {
            l_uid = n["uid"].as< std::string >();
        }

        assert( l_uid != "" && l_uid != "?" );

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
