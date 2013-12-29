/// -*- coding: utf-8 -*-
///
/// file: zmModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <mm/zmModel.h>

#include "zmThing.h"

#include <mm/zmTrace.h>
#include <mm/zmOsal.h>
#include <mm/zmOptions.h>

#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <stdlib.h>
using namespace zm;

zmOptions m_options;

void yamlToThingsMap( YAML::Node yamlNode, MindMatterModel::MindMatterModelMapType &thingsMap );

zm::MindMatterModel::MindMatterModel()
    : m_things              ()
    , m_localFolder         ( "" )
    , m_localModelFile      ( "" )
    , m_temporaryJournalFile( "" )
    , m_initialized         ( false )
    , m_dirty               ( false )
{
}

bool zm::MindMatterModel::equals(
        const zm::MindMatterModel &other,
        bool tell_why ) const
{
    /// prune if models differ in size
    if( m_things.size() != other.m_things.size() )
    {
        return false;
    }

    /// go through all elements of m_things - note that we don't have
    /// to do this for the second model
    BOOST_FOREACH( const MindMatterModelMapType::value_type& i, m_things )
    {
        /// find the key in the other map
        MindMatterModelMapType::left_const_iterator l_item_it( other.m_things.left.find( i.left ) );

        /// not found? return false!
        if( l_item_it == other.m_things.left.end() ) return false;

        /// values differ? return false!
        if( ! i.right->equals( *l_item_it->second, tell_why ) )
        {
            return false;
        }
    }

    /// if we reach this point the maps must be equal

    return true;
}

bool zm::MindMatterModel::operator==( const zm::MindMatterModel &other ) const
{
    return equals( other );
}

bool zm::MindMatterModel::operator!=( const zm::MindMatterModel &other ) const
{
    return ! equals( other );
}

ChangeSet zm::MindMatterModel::diff( const MindMatterModel &a_other ) const
{
    /// return a set of changes which describes what do we have to change
    /// on (*this) to get to (other)

    ChangeSet l_return;

    std::set< std::string > l_done_items;

    /// go through all elements of m_things
    BOOST_FOREACH( const MindMatterModelMapType::value_type& i, m_things )
    {
        const std::string &l_this_item_id(i.left);

        l_done_items.insert( l_this_item_id );

        /// find the key in the other map
        MindMatterModelMapType::left_const_iterator l_other_item_it(
                    a_other.m_things.left.find( l_this_item_id ) );

        if( l_other_item_it == a_other.m_things.left.end() )
        {
            l_return.add_remove_entry( l_this_item_id );
            continue;
        }

        if( *i.right != *l_other_item_it->second )
        {
            l_return.append(i.right->diff(
                                l_this_item_id,
                               *l_other_item_it->second));
        }
    }

    BOOST_FOREACH( const MindMatterModelMapType::value_type& i, a_other.m_things )
    {
        const std::string &l_other_item_id(i.left);

        /// prune items we handled already
        if( l_done_items.find( l_other_item_id ) != l_done_items.end() )
        {
            continue;
        }
        /// if we get here i must be an item which does not exist in
        /// (*this) - so just upate l_return to contain i

        /// insert i into l_return
        l_return.append( i.right->toDiff(l_other_item_id) );
    }

    return l_return;
}

void zm::MindMatterModel::setConfigPersistance( bool value )
{
    m_options.setAutosave( value );
}

void zm::MindMatterModel::setLocalFolder( const std::string &a_path )
{
    tracemessage( "privateDir: %s", a_path.c_str() );

    // handle cases: path contains "\"
    //               path == ""
    //               path == "/"
    //               path doesn't end with "/"
    // todo: there might be a boost::filesystem call for this

    std::string l_path = a_path;
    std::replace( l_path.begin(), l_path.end(), '\\', '/' );
    if( l_path == "/" ) l_path = "./";
    m_localFolder = l_path;
    m_options.load( m_localFolder + "/zm_config.json" );
}

void zm::MindMatterModel::addDomainSyncFolder( const std::string &domainName, const std::string &path )
{
    tracemessage( "new domain: %s %s", domainName.c_str(), path.c_str() );
}

bool zm::MindMatterModel::hasUsedUsername() const
{
    return m_options.hasValue( "username" )
            && m_options.getString( "username" ) != "";
}

bool zm::MindMatterModel::hasUsedHostname() const
{
    return m_options.hasValue( "hostname" )
            && m_options.getString( "hostname" ) != "";
}

void zm::MindMatterModel::setUsedUsername( const std::string &username )
{
    m_options.setString( "username", username );
}

void zm::MindMatterModel::setUsedHostname( const std::string &hostname )
{
    m_options.setString( "hostname", hostname );
}

void zm::MindMatterModel::initialize()
{
    srand( time( NULL ) * rand()  );

    /// find the name for the local model file - should be equal
    /// across sessions and unique for each client
    // <local folder>/zm-<user>-<client>-<zm-domain>-local.yaml
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-private-local.yaml
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolder << "/zm-"
                 << m_options.getString( "username" )
                 << "-"
                 << m_options.getString( "hostname" )
                 << "-local.yaml";

    m_localModelFile = l_ssFileName.str();

    /// find the name for the temporary session journal- should be equal
    /// across sessions and unique for each client
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-temp-journal.yaml
    std::stringstream l_ssTempJournalFile;
    l_ssTempJournalFile << m_localFolder << "/zm-"
                        << m_options.getString( "username" )
                        << "-"
                        << m_options.getString( "hostname" )
                        << "-journal-temp.yaml";
    m_temporaryJournalFile = l_ssTempJournalFile.str();

    if( boost::filesystem::exists( m_temporaryJournalFile ) )
    {
        /// there is a temporary journal file which should not be there
        /// (maybe a leftover after a crash or a debug session)
        /// make it available for the rest of the world
        makeTempJournalStatic();
    }

    tracemessage( "using temp file '%s'", m_temporaryJournalFile.c_str() );

    loadLocalModel( m_localModelFile );

    if( importJournalFiles() )
    {
        saveLocalModel( m_localModelFile );
    }

    m_initialized = true;
}

void zm::MindMatterModel::makeTempJournalStatic()
{
    std::stringstream l_ssJournalFile;
    l_ssJournalFile << "zm-"
                    << m_options.getString( "username" )
                    << "-"
                    << m_options.getString( "hostname" )
                    << "-"
                    << zm::common::time_stamp_iso()
                    << "-journal.yaml";

    std::string l_filename = l_ssJournalFile.str();
    m_options.addStringListElement( "read_journal", l_filename );

    l_filename = m_localFolder + "/" + l_filename;
    boost::filesystem::rename( m_temporaryJournalFile, l_filename );
}

void zm::MindMatterModel::localSave()
{
    /// just for debug purposes - later we will only write the journal
    saveLocalModel( m_localModelFile );

    //m_changeSet.write( m_temporaryJournalFile );
}

void zm::MindMatterModel::sync()
{
    saveLocalModel( m_localModelFile );

//    if( m_changeSet.write( m_temporaryJournalFile ) )
//    {
//        makeTempJournalStatic();
//    }

//    m_changeSet.clear();
}

void zm::MindMatterModel::merge( const std::string &a_modelFile )
{
    std::string l_modelFile = m_localFolder + "/" + a_modelFile;

    if( ! boost::filesystem::exists( l_modelFile ) )
    {
        return;
    }

    MindMatterModelMapType l_externalModel;

    YAML::Node l_import = YAML::LoadFile( l_modelFile );

    yamlToThingsMap( l_import, l_externalModel );

    BOOST_FOREACH( const MindMatterModelMapType::value_type& i, l_externalModel )
    {
        if( m_things.left.find( i.left ) == m_things.left.end() )
        {
            tracemessage( "external item not in local model: %s", i.left.c_str() );
            m_things.insert( MindMatterModelMapType::value_type(i.left,i.right) );
        }
    }
    saveLocalModel( m_localModelFile );
}

void zm::MindMatterModel::loadLocalModel( const std::string &filename )
{
    clear( m_things );

    if( ! boost::filesystem::exists( filename ) )
    {
        return;
    }
    YAML::Node l_import = YAML::LoadFile(filename);

    yamlToThingsMap( l_import, m_things );
}

bool zm::MindMatterModel::importJournalFiles()
{
    bool l_importedJournals = false;

    std::vector< std::string > l_journalFiles = getJournalFiles();
    tracemessage( "found %d journal files", l_journalFiles.size() );

    std::vector< std::string > l_alreadyImported = m_options.getStringList("read_journal");
    tracemessage( "%d journal files already imported", l_alreadyImported.size() );

    BOOST_FOREACH( std::string j, l_journalFiles )
    {
        std::string l_filename = boost::filesystem::path( j ).filename().string();
        if( std::find( l_alreadyImported.begin(), l_alreadyImported.end(), l_filename) == l_alreadyImported.end() )
        {
            tracemessage( "journal '%s' not imported yet", l_filename.c_str() );
            applyChangeSet( ChangeSet( j ) );
            m_options.addStringListElement( "read_journal", l_filename );
            l_importedJournals = true;
        }
    }
    return l_importedJournals;
}

void zm::MindMatterModel::applyChangeSet( const ChangeSet &changeSet )
{
    BOOST_FOREACH( const journal_ptr_t &j, changeSet.getJournal() )
    {
        MindMatterModelMapType::left_iterator l_item_it( m_things.left.find( j->item_uid ) );

        if( j->type == JournalItem::CreateItem && l_item_it != m_things.left.end() )
        {
            tracemessage( "WARNING: tried to create already existant item '%s'",
                          j->item_uid.c_str() );
            continue;
        }

        if( j->type != JournalItem::CreateItem && l_item_it == m_things.left.end() )
        {
            tracemessage( "WARNING: trying to modify item non existent item '%s'",
                          j->item_uid.c_str() );
            assert( j->type == JournalItem::CreateItem || l_item_it != m_things.left.end() );
        }

        switch( j->type )
        {
        case JournalItem::CreateItem:
            _createNewItem( j->item_uid, j->value, j->time );
            break;
        case JournalItem::SetStringValue:
            _setValue(l_item_it, j->key, j->value );
            break;
        case JournalItem::EraseItem:
            _eraseItem( l_item_it );
            break;
//        case JournalItem::AddAttribute:
//            _addTag( l_item_it, j->key );
//            break;
//        case JournalItem::RemoveAttribute:
//            _removeTag( l_item_it, j->key );
//            break;
        case JournalItem::ChangeCaption:
            _setCaption( l_item_it, j->value );
            break;
        case JournalItem::Connect:
        {
            MindMatterModelMapType::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to connect must exist");
            _connect( l_item_it, l_item2_it );
        } break;
        case JournalItem::Disconnect:
        {
            MindMatterModelMapType::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to disconnect from must exist");
            _disconnect( l_item_it, l_item2_it );
        } break;
        }
    }
}

std::vector< std::string > zm::MindMatterModel::getJournalFiles()
{
    std::vector< std::string > l_all_matching_files;

    const std::string target_path( m_localFolder );

    if( !boost::filesystem::exists( m_localFolder ) )
    {
        return l_all_matching_files;
    }

    const std::string my_filter( "*-journal.yaml" );


    boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
    for( boost::filesystem::directory_iterator i( target_path ); i != end_itr; ++i )
    {
        tracemessage( "%s", i->path().string().c_str() );
        // skip if not a file
        if( !boost::filesystem::is_regular_file( i->status() ) ) continue;

        // skip if no match
        if( !zm::common::matchesWildcards( i->path().string(), my_filter )) continue;

        // file matches, store it
        l_all_matching_files.push_back( i->path().string() );
    }

    std::sort( l_all_matching_files.begin(), l_all_matching_files.end() );

    return l_all_matching_files;
}

void zm::MindMatterModel::dirty()
{
    m_dirty = true;
}

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

void zm::MindMatterModel::saveLocalModel( const std::string &filename )
{
    /// be careful! if( !m_dirty ) return;


    if( !zm::common::create_base_directory( filename ) )
    {
        // todo: error
        return;
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

    BOOST_FOREACH(const MindMatterModelMapType::value_type& i, m_things)
    {
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "uid";
        l_yaml_emitter << YAML::Value << i.left;

        l_yaml_emitter << YAML::Key << "caption";
        l_yaml_emitter << YAML::Value << i.right->m_caption;

        l_yaml_emitter << YAML::Key << "hash1";
        l_yaml_emitter << YAML::Value << i.right->getHash();
/*
        if( ! i.right->m_attributes.empty() )
        {
            l_yaml_emitter << YAML::Key << "attributes";
            l_yaml_emitter << YAML::Value;
            l_yaml_emitter << YAML::BeginSeq;
            std::vector< std::string > v;
            BOOST_FOREACH( const std::string &a, i.right->m_attributes)
            {
                l_yaml_emitter << a;
            }
            l_yaml_emitter << YAML::EndSeq;
        }
*/
        if( ! i.right->m_string_values.empty() )
        {
            l_yaml_emitter << YAML::Key << "string_values";
            l_yaml_emitter << YAML::Value;
            /// this list is sorted anyway
            l_yaml_emitter << i.right->m_string_values;
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

const zm::MindMatterModel::MindMatterModelMapType & zm::MindMatterModel::things() const
{
    return m_things;
}

size_t zm::MindMatterModel::getItemCount() const
{
    return m_things.size();
}

void zm::MindMatterModel::clear( MindMatterModelMapType &thingsMap )
{
    BOOST_FOREACH(const MindMatterModelMapType::value_type& i, thingsMap)
    {
        delete i.right;
    }
    thingsMap.clear();
}

void yamlToThingsMap( YAML::Node yamlNode, zm::MindMatterModel::MindMatterModelMapType &thingsMap )
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

        MindMatter *l_new_thing = new MindMatter( l_caption );
        /*
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
        */
        if( n["string_values"] )
        {
            l_new_thing->m_string_values =
                n["string_values"].as< MindMatter::string_value_map_type >();

            BOOST_FOREACH(
                const MindMatter::string_value_map_type::value_type &a,
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

        thingsMap.insert( zm::MindMatterModel::MindMatterModelMapType::value_type( l_uid, l_new_thing ) );
    }
}

std::string zm::MindMatterModel::generateUid()
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

void zm::MindMatterModel::debug_dump() const
{
    BOOST_FOREACH(const MindMatterModelMapType::value_type& i, m_things)
    {
        tracemessage("%s",i.left.c_str());
    }
}
