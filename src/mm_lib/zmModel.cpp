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

static void yamlToThingsMap(
        YAML::Node      yamlNode,
        zm::MindMatterModel::uid_mm_bimap_t &thingsMap );

zm::MindMatterModel::MindMatterModel()
    : m_things              ()
    , m_old_things          ()
    , m_localFolder         ( "" )
    , m_localModelFile      ( "" )
    , m_localModelFileOld   ( "" )
    , m_initialized         ( false )
    , m_dirty               ( false )
{
}

bool zm::MindMatterModel::equals(
        const zm::MindMatterModel &a_other,
        bool tell_why ) const
{
    /// prune if models differ in size
    if( m_things.size() != a_other.m_things.size() )
    {
        return false;
    }

    /// go through all elements of m_things - note that we don't have
    /// to do this for the second model
    BOOST_FOREACH( const uid_mm_bimap_t::value_type& i, m_things )
    {
        /// find the key in the other map
        uid_mm_bimap_t::left_const_iterator l_item_it(
                    a_other.m_things.left.find( i.left ) );

        /// not found? return false!
        if( l_item_it == a_other.m_things.left.end() ) return false;

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

ChangeSet zm::MindMatterModel::diffTo( const MindMatterModel &a_other ) const
{
    return diff(m_things, a_other.things());
}

ChangeSet zm::MindMatterModel::diff(
        const uid_mm_bimap_t &model_from,
        const uid_mm_bimap_t &model_to )
{
    /// return a set of changes which describes what do we have to change
    /// on model_from to get to model_to

    ChangeSet l_return;

    std::set< std::string > l_done_items;

    /// go through all elements of m_things
    BOOST_FOREACH( const uid_mm_bimap_t::value_type& i, model_from )
    {
        const std::string &l_this_item_id(i.left);

        l_done_items.insert( l_this_item_id );

        /// find the key in the other map
        uid_mm_bimap_t::left_const_iterator l_other_item_it(
                    model_to.left.find( l_this_item_id ) );

        if( l_other_item_it == model_to.left.end() )
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

    BOOST_FOREACH( const uid_mm_bimap_t::value_type& i, model_to )
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

void zm::MindMatterModel::addDomainSyncFolder(
        const std::string &domainName,
        const std::string &path )
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

std::string zm::MindMatterModel::createJournalFileName() const
{
    std::stringstream l_ssJournalFile;
    l_ssJournalFile << "zm-"
                    << m_options.getString( "username" )
                    << "-"
                    << m_options.getString( "hostname" )
                    << "-"
                    << zm::common::time_stamp_iso()
                    << "-journal.yaml";

    return l_ssJournalFile.str();
}

std::string zm::MindMatterModel::createModelFileNameNew() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolder << "/zm-"
                 << m_options.getString( "username" )
                 << "-"
                 << m_options.getString( "hostname" )
                 << "-local.yaml";

    return l_ssFileName.str();
}

std::string zm::MindMatterModel::createModelFileNameOld() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolder << "/zm-"
                 << m_options.getString( "username" )
                 << "-"
                 << m_options.getString( "hostname" )
                 << "-local.yaml";

    return l_ssFileName.str();
}

void zm::MindMatterModel::initialize()
{
    srand( time( NULL ) * rand()  );

    /// find the name for the local model file - should be equal
    /// across sessions and unique for each client
    // <local folder>/zm-<user>-<client>-<zm-domain>-local.yaml
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-private-local.yaml

    m_localModelFile = createModelFileNameNew();

    m_localModelFileOld = createModelFileNameOld();

    /*
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
    */

    persistence_loadLocalModel();

    if( persistence_pullJournal() )
    {
        persistence_saveLocalModel();
    }

    m_initialized = true;
}

void zm::MindMatterModel::persistence_sync()
{
    // [TODO] - in case something goes wrong in here we should
    //          do all this in an atomic way

    /// save the current model for safety reasons
    persistence_saveLocalModel();

    /// load remote journals into current model
    persistence_pullJournal();

    /// write journal from old model to current model
    if( persistence_pushJournal() )
    {
        persistence_saveLocalModel();

        boost::filesystem::rename( m_localModelFile, m_localModelFileOld );

        // [TODO] - old model should be the synced to the new one here
    }
}

bool zm::MindMatterModel::loadModelFromFile(
        const std::string   &input_file,
        uid_mm_bimap_t      &thingsMap )
{
    clear( thingsMap );
    if( ! boost::filesystem::exists( input_file ) )
    {
        return false;
    }
    YAML::Node l_import = YAML::LoadFile(input_file);
    yamlToThingsMap( l_import, thingsMap );

    return true;
}

void zm::MindMatterModel::persistence_loadLocalModel()
{
    /// we try to load the new model file into the new model structure
    /// and the old file model into the old model structure
    ///
    /// in case the old model file does not exist, the new model file
    /// becomes the old one
    ///
    /// in case only the old model file exists it acts

    if( boost::filesystem::exists( m_localModelFileOld ) )
    {
        loadModelFromFile(m_localModelFileOld, m_old_things );

        if( boost::filesystem::exists( m_localModelFile ) )
        {
            loadModelFromFile(m_localModelFile, m_things );
        }
        else
        {
            loadModelFromFile(m_localModelFileOld, m_things );
        }
    }
    else
    {
        if( boost::filesystem::exists( m_localModelFile ) )
        {
            boost::filesystem::rename( m_localModelFile, m_localModelFileOld );

            loadModelFromFile(m_localModelFileOld, m_things );
            loadModelFromFile(m_localModelFileOld, m_old_things );
        }
    }
}

std::vector< std::string > zm::MindMatterModel::getHandledJournalFilenames()
{
    return m_options.getStringList("read_journal");
}

void zm::MindMatterModel::appendHandledJournalFilename(
        const std::string &a_filename)
{
    m_options.addStringListElement( "read_journal", a_filename );
}

bool zm::MindMatterModel::persistence_pullJournal()
{
    bool l_importedJournals = false;

    std::vector< std::string > l_journalFiles = getJournalFiles();
    tracemessage( "found %d journal files", l_journalFiles.size() );

    std::vector< std::string > l_alreadyImported =
            getHandledJournalFilenames();

    tracemessage( "%d journal files already imported",
                  l_alreadyImported.size() );

    BOOST_FOREACH( std::string j, l_journalFiles )
    {
        std::string l_filename = boost::filesystem::path( j ).filename().string();
        if( std::find( l_alreadyImported.begin(), l_alreadyImported.end(), l_filename) == l_alreadyImported.end() )
        {
            tracemessage( "journal '%s' not imported yet", l_filename.c_str() );
            applyChangeSet( ChangeSet( j ) );
            appendHandledJournalFilename(l_filename);
            l_importedJournals = true;
        }
    }
    return l_importedJournals;
}

bool zm::MindMatterModel::persistence_pushJournal()
{
    ChangeSet l_changeSet( diff( m_old_things, m_things ) );

    if(l_changeSet.isEmpty())
    {
        return false;
    }

    std::string l_journal_basename = createJournalFileName();

    std::string l_journal_fullname = m_localFolder + "/" + l_journal_basename;

    l_changeSet.write( l_journal_fullname );

    appendHandledJournalFilename(l_journal_basename);

    return true;
}

void zm::MindMatterModel::applyChangeSet( const ChangeSet &changeSet )
{
    BOOST_FOREACH( const journal_ptr_t &j, changeSet.getJournal() )
    {
        uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( j->item_uid ) );

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
            uid_mm_bimap_t::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to connect must exist");
            _connect( l_item_it, l_item2_it );
        } break;
        case JournalItem::Disconnect:
        {
            uid_mm_bimap_t::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to disconnect from must exist");
            _disconnect( l_item_it, l_item2_it );
        } break;
        }
    }
}

std::vector< std::string > zm::MindMatterModel::getJournalFiles() const
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

void zm::MindMatterModel::persistence_saveLocalModel()
{
    /// be careful! if( !m_dirty ) return;


    if( !zm::common::create_base_directory( m_localModelFile ) )
    {
        // todo: error
        return;
    }

    std::ofstream l_fout( m_localModelFile.c_str() );

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

    BOOST_FOREACH(const uid_mm_bimap_t::value_type& i, m_things)
    {
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "uid";
        l_yaml_emitter << YAML::Value << i.left;

        l_yaml_emitter << YAML::Key << "caption";
        l_yaml_emitter << YAML::Value << i.right->m_caption;

        l_yaml_emitter << YAML::Key << "hash1";
        l_yaml_emitter << YAML::Value << i.right->getHash();

        if( ! i.right->m_string_values.empty() )
        {
            l_yaml_emitter << YAML::Key << "string_values";
            l_yaml_emitter << YAML::Value;
            /// this list is sorted anyway
            l_yaml_emitter << i.right->m_string_values;
        }

        if( ! i.right->m_neighbours.empty() )
        {
            l_yaml_emitter << YAML::Key << "connections";
            l_yaml_emitter << YAML::Value;
            /// this list is sorted anyway
            l_yaml_emitter << i.right->getNeighbourUids();
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

const zm::MindMatterModel::uid_mm_bimap_t & zm::MindMatterModel::things() const
{
    return m_things;
}

size_t zm::MindMatterModel::getItemCount() const
{
    return m_things.size();
}

void zm::MindMatterModel::clear( uid_mm_bimap_t &thingsMap )
{
    BOOST_FOREACH(const uid_mm_bimap_t::value_type& i, thingsMap)
    {
        delete i.right;
    }
    thingsMap.clear();
}

void yamlToThingsMap(
        YAML::Node                           yamlNode,
        zm::MindMatterModel::uid_mm_bimap_t &thingsMap )
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
            if(n["hash1"].as< std::string >() != l_new_thing->getHash(true))
            {
                tracemessage("saved and loaded hashes differ!");
                assert( n["hash1"].as< std::string >() == l_new_thing->getHash() );
            }
        }
        assert( l_new_thing->hasValue("global_time_created") );

        thingsMap.insert( zm::MindMatterModel::uid_mm_bimap_t::value_type( l_uid, l_new_thing ) );
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
    BOOST_FOREACH(const uid_mm_bimap_t::value_type& i, m_things)
    {
        tracemessage("%s",i.left.c_str());
    }
}
