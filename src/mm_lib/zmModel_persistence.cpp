/// -*- coding: utf-8 -*-
///
/// file: zmModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <mm/zmModel.h>

#include "zmThing.h"

#include <mm/zmTrace.h>

#include <yaml-cpp/yaml.h>

#include <boost/filesystem.hpp>

using namespace zm;

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
        const std::string   &a_input_file,
        ModelData           &a_thingsMap,
        bool                 a_checkHashes)
{
    clear( a_thingsMap );

    if( ! boost::filesystem::exists( a_input_file ) )
    {
        return false;
    }
    std::ifstream l_yaml_stream(a_input_file.c_str());

    if(!l_yaml_stream)
    {
        return false;
    }

    YAML::Node l_import = YAML::Load(l_yaml_stream);
    yamlToThingsMap( l_import, a_thingsMap, a_checkHashes );

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

    trace_i("load new  file: %s", m_localModelFile.c_str());
    trace_i("load sync file: %s", m_localModelFileSynced.c_str());

    bool l_result = false;

    if( boost::filesystem::exists( m_localModelFileSynced ) )
    {
        l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things_synced, m_checkHashes );

        if( boost::filesystem::exists( m_localModelFile ) )
        {
            l_result |=
                loadModelFromFile(m_localModelFile, m_things, m_checkHashes );
        }
        else
        {
            l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things, m_checkHashes );
        }
    }
    else
    {
        if( boost::filesystem::exists( m_localModelFile ) )
        {
            boost::filesystem::rename( m_localModelFile, m_localModelFileSynced );

            l_result |=
                loadModelFromFile(m_localModelFileSynced, m_things, m_checkHashes );

            deepCopy(m_things, m_things_synced);
        }
    }
    return l_result;
}

bool zm::MindMatterModel::persistance_loadSnapshot()
{
    std::set<std::string> l_snapshotfiles =
            zm::common::get_files_in_dir(
                m_localFolderSync, "snapshot-*.yaml");

    if(l_snapshotfiles.empty())
    {
        return false;
    }

    const std::string &l_filename = *(l_snapshotfiles.rbegin());

    loadModelFromFile(l_filename, m_things, m_checkHashes);

    deepCopy(m_things, m_things_synced);

    return true;
}

bool zm::MindMatterModel::persistance_createSnapshot()
{
    _saveModel(m_things, createSnapshotFileName());

    return true;
}

ChangeSet zm::MindMatterModel::persistence_pullJournal()
{
    assert( equals(m_things, m_things_synced, true) );

    ChangeSet l_result;
    bool l_importedJournals = false;

    std::vector< std::string > l_journalFiles = getJournalFiles();
    trace_i( "found %d journal files", l_journalFiles.size() );

    const std::set< std::string > & l_alreadyImported =
            m_things.m_read_journals;

    trace_i( "%d journal files already imported",
                  l_alreadyImported.size() );

    for( const std::string &j: l_journalFiles )
    {
        std::string l_filename = boost::filesystem::path( j ).filename().string();
        if( std::find( l_alreadyImported.begin(), l_alreadyImported.end(), l_filename) == l_alreadyImported.end() )
        {
            trace_i( "journal '%s' not imported yet", l_filename.c_str() );
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
        trace_i( "no changes - don't write journal" );
        return l_changeSet;
    }

    std::string l_journal_basename = createJournalFileName();

    std::string l_journal_fullname = m_localFolderSync + "/" + l_journal_basename;

    trace_i( "%d changes - write journal '%s'",
                  l_changeSet.size(),
                  l_journal_basename.c_str() );

    l_changeSet.write( l_journal_fullname );

    m_things.m_read_journals.insert(l_journal_basename);

    deepCopy(m_things, m_things_synced);

    return l_changeSet;
}

void zm::MindMatterModel::applyChangeSet( const ChangeSet &changeSet )
{
    for( const journal_ptr_t &j: changeSet.getJournal() )
    {
        ModelData::left_iterator l_item_it( m_things.left.find( j->item_uid ) );

        if( j->type == JournalItem::CreateItem && l_item_it != m_things.left.end() )
        {
            trace_i( "WARNING: tried to create already existant item '%s'",
                          j->item_uid.c_str() );
            continue;
        }

        if( j->type != JournalItem::CreateItem && l_item_it == m_things.left.end() )
        {
            trace_i( "WARNING: trying to modify item non existent item '%s'",
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
                        m_things.left.find( j->key ) );
            assert( l_item2_it != m_things.left.end() &&
                    "item to connect must exist");
            //_connectDuplex( l_item_it, l_item2_it, Directed );
            _connectSingle( l_item_it, l_item2_it, atoi(j->value.c_str()));
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
        trace_i( "%s", i->path().string().c_str() );
        // skip if not a file
        if( !boost::filesystem::is_regular_file( i->status() ) )
        {
            continue;
        }

        // skip if no match
        if( !zm::common::matchesWildcards( i->path().string(), my_filter ))
        {
            continue;
        }

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
    _saveModel(m_things, m_localModelFile);
}

void zm::MindMatterModel::_saveModel(
        const ModelData &a_model,
        const std::string &a_filename)
{
    if( !zm::common::create_base_directory( a_filename ) )
    {
        // [todo]: handle/show error
        return;
    }

    std::ofstream l_fout( a_filename.c_str() );

    assert( l_fout.is_open() );

    if( a_model.empty() )
    {
        return;
    }

    /// NOTE: we do the yaml exporting here semi manually
    ///       because this way we can ensure consistent
    ///       order of items between savings

    YAML::Emitter l_yaml_emitter( l_fout );

    l_yaml_emitter << YAML::BeginSeq;

    for(const ModelData::value_type& i: a_model)
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
            /*
            std::map<std::string, int> l_temp_neighbour_map;

            for(const std::pair<uid_t, int> &a: i.right->getNeighbours())
            {
                l_temp_neighbour_map[a] = 0;
            }
            */

            l_yaml_emitter << YAML::Key << "connections";
            l_yaml_emitter << YAML::Value;
            l_yaml_emitter << i.right->getNeighbours();
        }
        l_yaml_emitter << YAML::EndMap;
    }

    for(const std::string &i: a_model.m_read_journals)
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

void zm::MindMatterModel::yamlToThingsMap(
        const YAML::Node    &a_yamlNode,
        ModelData           &a_thingsMap,
        bool                a_checkHashes)
{
    typedef std::map< uid_t, int > conn_t;

    std::map< uid_t, conn_t > l_connection_uids;
    std::map< uid_t, std::vector< uid_t > > l_tag_names;
    std::map< uid_t, std::string > l_hashes;

    for( YAML::Node n: a_yamlNode )
    {
        // means 'uid' in n xor 'read' in n
        assert( !(n["uid"]) != !(n["read"]) );

        if( n["uid"] )
        {
            assert( n["caption"] );

            uid_t l_uid = n.Tag();

            if( n["uid"] )
            {
                l_uid = n["uid"].as< uid_t >();
            }

            assert( l_uid != "" && l_uid != "?" );

            std::string l_caption = n["caption"].as< std::string >();

            trace_i("caption: '%s'", l_caption.c_str());

            MindMatter *l_new_thing = new MindMatter( l_caption );

            if( n["connections"] )
            {
                try
                {
                    l_connection_uids[l_uid] = n["connections"].as< conn_t >();
                }
                catch( YAML::Exception &e )
                {
                    conn_t l_connections;
                    /// fallback to older syntax
                    for( const uid_t &l_conn: n["connections"].as< std::vector< uid_t > >())
                    {
                        trace_i("%s", l_conn.c_str());
                        l_connections[l_conn] = 0;
                    }
                    l_connection_uids[l_uid] = l_connections;
                }

            }

            /// this is deprecated and just ensures that older models can
            /// be read
            if( n["attributes"] )
            {
                l_tag_names[l_uid] =
                        n["attributes"].as< std::vector< uid_t > >();
            }

            if( n["string_values"] )
            {
                l_new_thing->m_string_values =
                    n["string_values"].as< string_value_map_type >();

                for( const string_value_map_type::value_type &a:
                    l_new_thing->m_string_values )
                {
                    trace_i("%s: %d", a.first.c_str(), a.second.c_str());
                }
            }

            if( n["hash1"] )
            {
                l_hashes[l_uid] = n["hash1"].as< std::string >();
            }
            assert( l_new_thing->hasValue("global_time_created")
                    || l_uid == l_new_thing->m_caption );

            a_thingsMap.insert(
                        zm::MindMatterModel::ModelData::value_type(
                            l_uid, l_new_thing ) );
        }
        else if(n["read"])
        {
            std::string bla = n["read"].as< std::string >();
            a_thingsMap.m_read_journals.insert(bla);
        }
    }

    /// since we could not fully process all items yet - connections
    /// could not be established due to incomplete list of items, we
    /// postprocess them now and check there hash values
    for(const zm::MindMatterModel::ModelData::value_type &i: a_thingsMap)
    {
        const std::string &l_uid( i.left );
        zm::MindMatter *l_item( i.right );

        ///
        /// handle connections
        ///
        std::map< uid_t, conn_t >::const_iterator
                l_connections_it = l_connection_uids.find(l_uid);

        if(l_connections_it != l_connection_uids.end())
        {
            for( const neighbour_t &other_uid: l_connections_it->second)
            {
                zm::MindMatterModel::ModelData::left_iterator
                        l_other_it( a_thingsMap.left.find( other_uid.first ) );

                assert(l_other_it != a_thingsMap.left.end());

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
                        a_thingsMap.left.find( l_uid ) );

            for( const std::string &tag_name: l_tags_it->second)
            {
                _addTag(a_thingsMap, l_item_left_it, tag_name);
            }
        }

        ///
        /// handle hashes
        ///

        /// tag might have been generated and has no hash yet
        if(a_checkHashes and l_uid != l_item->m_caption)
        {
            std::map< std::string, std::string >::const_iterator
                    l_hash_it = l_hashes.find(l_uid);

            assert(l_hash_it != l_hashes.end());

            if(l_hash_it->second != l_item->createHash())
            {
                trace_i("saved and loaded hashes differ!");
                trace_i("'%s' != '%s'",
                             l_hash_it->second.c_str(),
                             l_item->createHash(true).c_str());
                assert(l_hash_it->second == l_item->createHash());
            }
        }
    }

    // _debug_dump(thingsMap);
}

