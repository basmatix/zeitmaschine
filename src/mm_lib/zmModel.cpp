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

//static void _debug_dump(
//        const zm::MindMatterModel::ModelData &thingsMap);

zm::MindMatterModel::MindMatterModel()
    : m_things                  ()
    , m_things_synced           ()
    , m_localFolderRoot         ( "" )
    , m_localFolderSync         ( "" )
    , m_localModelFile          ( "" )
    , m_localModelFileSynced    ( "" )
    //, m_read_journals           ()
    , m_initialized             ( false )
    , m_options                 ( new zm::zmOptions )
{
}

zm::MindMatterModel::~MindMatterModel()
{
    clear(m_things_synced);
    clear(m_things);
    delete m_options;
}

bool zm::MindMatterModel::equals(
        const ModelData &a_first,
        const ModelData &a_second,
        bool tell_why )
{
    /// prune if models differ in size
    if( a_first.size() != a_second.size() )
    {
        return false;
    }

    /// go through all elements of m_things - note that we don't have
    /// to do this for the second model
    BOOST_FOREACH( const ModelData::value_type& i, a_first )
    {
        /// find the key in the other map
        ModelData::left_const_iterator l_item_it(
                    a_second.left.find( i.left ) );

        /// not found? return false!
        if( l_item_it == a_second.left.end() )
        {
            return false;
        }

        /// values differ? return false!
        if( ! i.right->equals( *l_item_it->second, tell_why ) )
        {
            if(tell_why)
            {
                tracemessage("at '%s'", i.left.c_str() );
            }
            return false;
        }
    }

    /// if we reach this point the maps must be equal

    return true;
}

bool zm::MindMatterModel::equals(
        const zm::MindMatterModel &a_other,
        bool tell_why ) const
{
    return equals(m_things, a_other.m_things, tell_why);
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
        const ModelData &model_from,
        const ModelData &model_to )
{
    /// return a set of changes which describes what do we have to change
    /// on model_from to get to model_to

    ChangeSet l_return;

    std::set< std::string > l_done_items;

    /// go through all elements of m_things
    BOOST_FOREACH( const ModelData::value_type& i, model_from )
    {
        const std::string &l_this_item_id(i.left);

        l_done_items.insert( l_this_item_id );

        /// find the key in the other map
        ModelData::left_const_iterator l_other_item_it(
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

    BOOST_FOREACH( const ModelData::value_type& i, model_to )
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
    m_options->setAutosave( value );
}

const std::string & zm::MindMatterModel::getLocalFolder() const
{
    return m_localFolderRoot;
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

    m_localFolderRoot = l_path;
    m_localFolderSync = (boost::filesystem::path(m_localFolderRoot) / "sync").string();

    m_options->load( m_localFolderRoot + "/zm_config.json" );
}

void zm::MindMatterModel::addDomainSyncFolder(
        const std::string &domainName,
        const std::string &path )
{
    tracemessage( "new domain: %s %s", domainName.c_str(), path.c_str() );
}

bool zm::MindMatterModel::hasUsedUsername() const
{
    return m_options->hasValue( "username" )
        && m_options->getString( "username" ) != "";
}

bool zm::MindMatterModel::hasUsedHostname() const
{
    return m_options->hasValue( "hostname" )
        && m_options->getString( "hostname" ) != "";
}

void zm::MindMatterModel::setUsedUsername( const std::string &username )
{
    m_options->setString( "username", username );
}

void zm::MindMatterModel::setUsedHostname( const std::string &hostname )
{
    m_options->setString( "hostname", hostname );
}

std::string zm::MindMatterModel::createJournalFileName() const
{
    int l_counter = 0;
    std::string l_result;

    do
    {
        std::stringstream l_ssJournalFile;
        l_ssJournalFile << "zm-"
                        << m_options->getString( "username" )
                        << "-"
                        << m_options->getString( "hostname" )
                        << "-"
                        << zm::common::time_stamp_iso()
                        << "_"
                        << l_counter++
                        << "-journal.yaml";
        l_result = l_ssJournalFile.str();
    }
    while(boost::filesystem::exists(m_localFolderSync + "/" + l_result));

    return l_result;
}

std::string zm::MindMatterModel::createModelFileNameNew() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolderRoot << "/zm-"
                 << m_options->getString( "username" )
                 << "-"
                 << m_options->getString( "hostname" )
                 << "-local.yaml";

    return l_ssFileName.str();
}

std::string zm::MindMatterModel::createModelFileNameOld() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolderRoot << "/zm-"
                 << m_options->getString( "username" )
                 << "-"
                 << m_options->getString( "hostname" )
                 << "-local-last_synced.yaml";

    return l_ssFileName.str();
}

std::string zm::MindMatterModel::createJournalListFileName() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolderRoot << "/zm-read_journals.txt";

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

    m_localModelFileSynced = createModelFileNameOld();

    if( !persistence_loadLocalModel() )
    {
        persistance_loadBaseLine();
    }

    // this is currently not possible since a pull can not be done
    // without a prior push (which is a full sync, which should only
    // be done intentionally)
//    if( !persistence_pullJournal().isEmpty() )
//    {
//        persistence_saveLocalModel();
//    }

    debug_dump();

    m_initialized = true;
}

bool zm::MindMatterModel::persistence_sync()
{
    // [TODO] - in case something goes wrong in here we should
    //          do all this in an atomic way

    /// NOTE: we have to solve a tricky problem here. in case we want
    ///       to achieve a 'pull only' approach without the need to
    ///       write a journal we need to apply the remote journals to
    ///       both the new and the old model. otherwise on the next journal
    ///       export we would replicate the journaling information we just
    ///       imported when.
    ///       Importing the jounals on both models would be tricky on the
    ///       other hand because conflict resolving would have to take place
    ///       twice, too.

    /// save the current model for safety reasons
    persistence_saveLocalModel();

    /// write journal from old model to current model
    ChangeSet l_exportedChanges = persistence_pushJournal();

    /// load remote journals into current model
    ChangeSet l_importedChanges = persistence_pullJournal();

    if( l_exportedChanges.isEmpty() && l_importedChanges.isEmpty() )
    {
        return false;
    }

    //[TODO]
    //resolveConflicts(l_exportedChanges, l_importedChanges);

    persistence_saveLocalModel();

    boost::filesystem::rename( m_localModelFile, m_localModelFileSynced );

    return true;
}

bool zm::MindMatterModel::loadModelFromFile(
        const std::string   &input_file,
        ModelData      &thingsMap )
{
    clear( thingsMap );

    if( ! boost::filesystem::exists( input_file ) )
    {
        return false;
    }
    std::ifstream l_yaml_stream(input_file.c_str());

    if(!l_yaml_stream)
    {
        return false;
    }

    YAML::Node l_import = YAML::Load(l_yaml_stream);
    yamlToThingsMap( l_import, thingsMap );

    return true;
}

bool zm::MindMatterModel::persistence_loadLocalModel()
{
    /// we try to load the new model file into the new model structure
    /// and the old file model into the old model structure
    ///
    /// in case the old model file does not exist, the new model file
    /// becomes the old one
    ///
    /// in case only the old model file exists it acts

    tracemessage("load new  file: %s", m_localModelFile.c_str());
    tracemessage("load sync file: %s", m_localModelFileSynced.c_str());

    bool l_result = false;

    if( boost::filesystem::exists( m_localModelFileSynced ) )
    {
        l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things_synced );

        if( boost::filesystem::exists( m_localModelFile ) )
        {
            l_result |=
                loadModelFromFile(m_localModelFile, m_things );
        }
        else
        {
            l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things );
        }
    }
    else
    {
        if( boost::filesystem::exists( m_localModelFile ) )
        {
            boost::filesystem::rename( m_localModelFile, m_localModelFileSynced );

            l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things );

            deepCopy(m_things, m_things_synced);
        }
    }
    return l_result;
}

bool zm::MindMatterModel::persistance_loadBaseLine()
{
    return false;
}

bool zm::MindMatterModel::persistance_loadCreateBaseLine()
{
    return false;
}
/*
const std::set< std::string > & zm::MindMatterModel::getHandledJournalFilenames()
{
    if(m_read_journals.empty())
    {
        std::ifstream l_file(createJournalListFileName().c_str());
        if(!l_file.is_open())
        {
            return m_read_journals;
        }
        std::string l_line;
        while (std::getline(l_file, l_line))
        {
            m_read_journals.insert(l_line);
        }
    }
    return m_read_journals;
}
void zm::MindMatterModel::appendHandledJournalFilename(
        const std::string &a_filename)
{
    m_read_journals.insert(a_filename);

    std::ofstream l_file(createJournalListFileName().c_str());

    BOOST_FOREACH( const std::string &l_line, m_read_journals)
    {
        l_file << l_line << std::endl;
    }
}
*/

ChangeSet zm::MindMatterModel::persistence_pullJournal()
{
    assert( equals(m_things, m_things_synced, true) );

    ChangeSet l_result;
    bool l_importedJournals = false;

    std::vector< std::string > l_journalFiles = getJournalFiles();
    tracemessage( "found %d journal files", l_journalFiles.size() );

    const std::set< std::string > & l_alreadyImported =
            m_things.m_read_journals;

    tracemessage( "%d journal files already imported",
                  l_alreadyImported.size() );

    BOOST_FOREACH( const std::string &j, l_journalFiles )
    {
        std::string l_filename = boost::filesystem::path( j ).filename().string();
        if( std::find( l_alreadyImported.begin(), l_alreadyImported.end(), l_filename) == l_alreadyImported.end() )
        {
            tracemessage( "journal '%s' not imported yet", l_filename.c_str() );
            applyChangeSet( ChangeSet( j ) );
            m_things.m_read_journals.insert(l_filename);
            l_importedJournals = true;
        }
    }

    if( l_importedJournals )
    {
        l_result = diff( m_things_synced, m_things );
        deepCopy(m_things, m_things_synced);
    }
    return l_result;
}

ChangeSet zm::MindMatterModel::persistence_pushJournal()
{
    ChangeSet l_changeSet( diff( m_things_synced, m_things ) );

    if(l_changeSet.isEmpty())
    {
        tracemessage( "no changes - don't write journal" );
        return l_changeSet;
    }

    std::string l_journal_basename = createJournalFileName();

    std::string l_journal_fullname = m_localFolderSync + "/" + l_journal_basename;

    tracemessage( "%d changes - write journal '%s'",
                  l_changeSet.size(),
                  l_journal_basename.c_str() );

    l_changeSet.write( l_journal_fullname );

    m_things.m_read_journals.insert(l_journal_basename);

    deepCopy(m_things, m_things_synced);

    return l_changeSet;
}

void zm::MindMatterModel::applyChangeSet( const ChangeSet &changeSet )
{
    BOOST_FOREACH( const journal_ptr_t &j, changeSet.getJournal() )
    {
        ModelData::left_iterator l_item_it( m_things.left.find( j->item_uid ) );

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
            _createNewItem( m_things, j->item_uid, j->value, j->time );
            break;
        case JournalItem::SetStringValue:
            _setValue(l_item_it, j->key, j->value );
            break;
        case JournalItem::EraseItem:
            _eraseItem( l_item_it );
            break;
        case JournalItem::ChangeCaption:
            _setCaption( l_item_it, j->value );
            break;
        case JournalItem::Connect:
        {
            ModelData::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to connect must exist");
            _connect( l_item_it, l_item2_it );
        } break;
        case JournalItem::Disconnect:
        {
            ModelData::left_iterator l_item2_it(
                        m_things.left.find( j->value ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to disconnect from must exist");
            _disconnect( l_item_it, l_item2_it );
        } break;
        case JournalItem::AddAttribute:
        {
            _addTag(m_things, l_item_it, j->key);
        } break;
        }
    }
}

std::vector< std::string > zm::MindMatterModel::getJournalFiles() const
{
    std::vector< std::string > l_all_matching_files;

    const std::string target_path( m_localFolderSync );

    if( !boost::filesystem::exists( target_path ) )
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

// info regarding string encoding:
//    http://code.google.com/p/yaml-cpp/wiki/Strings

void zm::MindMatterModel::persistence_saveLocalModel()
{
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

    BOOST_FOREACH(const ModelData::value_type& i, m_things)
    {
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "uid";
        l_yaml_emitter << YAML::Value << i.left;

        l_yaml_emitter << YAML::Key << "caption";
        l_yaml_emitter << YAML::Value << i.right->m_caption;

        l_yaml_emitter << YAML::Key << "hash1";
        l_yaml_emitter << YAML::Value << i.right->createHash();

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

    BOOST_FOREACH(const std::string &i, m_things.m_read_journals)
    {
        l_yaml_emitter << YAML::BeginMap;

        l_yaml_emitter << YAML::Key << "read";
        l_yaml_emitter << YAML::Value << i;

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

const zm::MindMatterModel::ModelData & zm::MindMatterModel::things() const
{
    return m_things;
}

size_t zm::MindMatterModel::getItemCount() const
{
    return m_things.size();
}

void zm::MindMatterModel::clear( ModelData &thingsMap )
{
    BOOST_FOREACH(const ModelData::value_type& i, thingsMap)
    {
        delete i.right;
    }
    thingsMap.clear();
}

void zm::MindMatterModel::yamlToThingsMap(
        const YAML::Node    &yamlNode,
        ModelData      &thingsMap )
{
    std::map< std::string, std::vector< std::string> > l_connection_uids;
    std::map< std::string, std::vector< std::string> > l_tag_names;
    std::map< std::string, std::string > l_hashes;

    BOOST_FOREACH( YAML::Node n, yamlNode )
    {
        // [todo] should be xor
        assert( n["uid"] || n["read"] );

        if( n["uid"] )
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

            if( n["connections"] )
            {
                l_connection_uids[l_uid] =
                        n["connections"].as< std::vector< std::string > >();
            }

            /// this is deprecated and just ensures that older models can
            /// be read
            if( n["attributes"] )
            {
                l_tag_names[l_uid] =
                        n["attributes"].as< std::vector< std::string > >();
            }

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
                l_hashes[l_uid] = n["hash1"].as< std::string >();
            }
            assert( l_new_thing->hasValue("global_time_created")
                    || l_uid == l_new_thing->m_caption );

            thingsMap.insert(
                        zm::MindMatterModel::ModelData::value_type(
                            l_uid, l_new_thing ) );
        }
        /*
        else if(n["read"])
        {
            std::string bla = n["read"].as< std::string >();
            m_read_journals.insert(bla);
        }
        */
    }

    /// since we could not fully process all items yet - connections
    /// could not be established due to incomplete list of items, we
    /// postprocess them now and check there hash values
    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type &i,
                  thingsMap)
    {
        const std::string &l_uid( i.left );
        zm::MindMatter *l_item( i.right );

        ///
        /// handle connections
        ///
        std::map< std::string, std::vector< std::string> >::const_iterator
                l_connections_it = l_connection_uids.find(l_uid);

        if(l_connections_it != l_connection_uids.end())
        {
            BOOST_FOREACH( const std::string &other_uid, l_connections_it->second)
            {
                zm::MindMatterModel::ModelData::left_iterator
                        l_other_it( thingsMap.left.find( other_uid ) );

                assert(l_other_it != thingsMap.left.end());

                l_item->m_neighbours[l_other_it->second] = other_uid;
            }
        }

        ///
        /// handle tags (deprecated)
        ///
        std::map< std::string, std::vector< std::string> >::const_iterator
                l_tags_it = l_tag_names.find(l_uid);

        if(l_tags_it != l_tag_names.end())
        {
            ModelData::left_iterator l_item_left_it(
                        thingsMap.left.find( l_uid ) );

            BOOST_FOREACH( const std::string &tag_name, l_tags_it->second)
            {
                _addTag(thingsMap, l_item_left_it, tag_name);
            }
        }

        ///
        /// handle hashes
        ///

        /// tag might have been generated and has no hash yet
        if(l_uid != l_item->m_caption)
        {
            std::map< std::string, std::string >::const_iterator
                    l_hash_it = l_hashes.find(l_uid);

            assert(l_hash_it != l_hashes.end());

            if(l_hash_it->second != l_item->createHash())
            {
                tracemessage("saved and loaded hashes differ!");
                assert(l_hash_it->second == l_item->createHash());
            }
        }
    }

    // _debug_dump(thingsMap);
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

std::string zm::MindMatterModel::createHash( bool verbose) const
{
    size_t l_hash(0);

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type &i,
                  m_things)
    {
        boost::hash_combine(l_hash, i.left);
        boost::hash_combine(l_hash, i.right->createHash());
    }

    std::stringstream l_result_stream;
    l_result_stream << std::hex << l_hash;

    return l_result_stream.str();
}

void zm::MindMatterModel::deepCopy(
        const ModelData &a_source,
              ModelData &a_dest)
{
    clear(a_dest);

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type &i,
                  a_source)
    {
        MindMatter *l_new_thing = new MindMatter(i.right->m_caption);
        l_new_thing->m_string_values = i.right->m_string_values;

        a_dest.insert(
                    zm::MindMatterModel::ModelData::value_type(
                        i.left, l_new_thing ) );
    }
    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type &i,
                  a_source)
    {
        ModelData::left_iterator l_dest_first_item_it(
                    a_dest.left.find( i.left ) );

        assert( l_dest_first_item_it != a_dest.left.end() );

        BOOST_FOREACH(const MindMatter::item_uid_pair_t l_neighbour,
                      i.right->m_neighbours)
        {
            ModelData::left_iterator l_dest_second_item_it(
                        a_dest.left.find( l_neighbour.second ) );

            assert( l_dest_second_item_it != a_dest.left.end() );

            _connect(l_dest_first_item_it, l_dest_second_item_it);
        }
    }
}

void zm::MindMatterModel::duplicateModelTo(MindMatterModel &other) const
{
    deepCopy(m_things, other.m_things);
    deepCopy(m_things, other.m_things_synced);
}


void zm::MindMatterModel::ModelData::debug_dump() const
{
    tracemessage(">> dump");

    BOOST_FOREACH(const zm::MindMatterModel::ModelData::value_type& i,
                  m_data)
    {
        std::ostringstream l_neighbours;
        l_neighbours << "(";
        BOOST_FOREACH( const MindMatter::item_uid_pair_t &m,
                       i.right->m_neighbours)
        {
            l_neighbours << m.second << ", ";
        }
        l_neighbours << ")";
        tracemessage("  %s (#%s) %s '%s'",
                     i.left.c_str(),
                     i.right->createHash().c_str(),
                     l_neighbours.str().c_str(),
                     i.right->m_caption.c_str());
    }
}

void zm::MindMatterModel::debug_dump() const
{
    m_things.debug_dump();
}
