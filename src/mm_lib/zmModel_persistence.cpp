/// -*- coding: utf-8 -*-
///
/// file: zmModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "zmThing.h"
#include <mm/zmModel.h>
#include <mm/zmTrace.h>

#include <yaml-cpp/yaml.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

using namespace zm;

std::vector< std::string > zm::MindMatterModel::diff() const
{
    ChangeSet l_stash( _diff( m_things_synced, m_things ) );

    std::vector< std::string > l_result;
    for( const journal_ptr_t &j: l_stash.getJournal() )
    {

        l_result.push_back( boost::str(
                                boost::format("%s '%s'")
                                              % j->item_uid
                                              % getCaption(j->item_uid)));
    }
    return l_result;
}

std::vector< std::string > zm::MindMatterModel::diffRemote() const
{
    std::list< std::string > l_journalsToImport(findNewJournals());

    std::vector< std::string > l_result;
    for( const std::string &l_filename: l_journalsToImport )
    {
        l_result.push_back(l_filename);
    }

    return l_result;
}

bool zm::MindMatterModel::sync_pull()
{
    /// - current and last synced model may differ
    /// - after pull two local files may be saved

    if(findNewJournals().empty())
    {
        /// in case there are no new journal files we can stop here
        return false;
    }

    // TODO - allow non-persistent run (implies handled journal files to be
    //        stored in the changeset)

    /// stash local changes (like in git)
    ChangeSet l_stash( _diff( m_things_synced, m_things ) );

    /// sync current with last synced model
    deepCopy(m_things_synced, m_things);

    /// pull remote changes - after this m_things_synced, m_things are
    /// equal again
    ChangeSet l_importedChanges = _persistence_pullJournal();

    deepCopy(m_things, m_things_synced);

    _saveModel(m_things_synced, m_localModelFileSynced);

    //[TODO] -
    //resolveConflicts(l_stash, l_importedChanges);

    _applyChangeSet(m_things, l_stash);

    //persistence_saveLocalModel();

    //boost::filesystem::rename( m_localModelFile, m_localModelFileSynced );

    return ! l_importedChanges.isEmpty();
}

bool zm::MindMatterModel::sync_push()
{
    /// - remote changes must not exist (to resolve conflicts locally)
    /// - current and last synced model should differ (otherwise makes no sense)
    /// - after push only the last synced model file should exist

    if( ! findNewJournals().empty())
    {
        /// in case there are journal files to be imported we abort
        return false;
    }

    return ! _persistence_pushJournal().isEmpty();
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
            deepCopy(m_things_synced, m_things);
        }
    }
    else
    {
        if( boost::filesystem::exists( m_localModelFile ) )
        {
            /// NOTE: m_things_synced has to be clear if no last-synced-file
            ///       can be found (rather than done before at this place)
            ///       Otherwise we implicitly assume that all changes being
            ///       done have already been synced

            l_result |=
                loadModelFromFile(m_localModelFile, m_things, m_checkHashes );

            clear( m_things_synced );
        }
    }
    return l_result;
}

bool zm::MindMatterModel::persistence_loadSnapshot()
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

bool zm::MindMatterModel::persistence_createSnapshot()
{
    _saveModel(m_things, createSnapshotFileName());

    return true;
}

std::list <std::string > zm::MindMatterModel::findNewJournals() const
{
    std::list <std::string > l_result;

    const std::set< std::string > &l_alreadyImported(m_things.m_read_journals);

    for( const std::string &l_filename: getJournalFiles() )
    {
        std::string l_basename(
                    boost::filesystem::path(
                        l_filename).filename().string());
        if( std::find(
                    l_alreadyImported.begin(),
                    l_alreadyImported.end(),
                    l_basename) == l_alreadyImported.end() )
        {
            l_result.push_back(l_filename);
        }
    }

    return l_result;
}

ChangeSet zm::MindMatterModel::_persistence_pullJournal()
{
    trace_assert_h( equals(m_things, m_things_synced, true) );

    std::list <std::string > l_journalsToImport(findNewJournals());

    trace_i( "found %d new journal files", l_journalsToImport.size() );

    for( const std::string &l_filename: l_journalsToImport )
    {
        std::string l_basename(
                    boost::filesystem::path(
                        l_filename).filename().string());
        trace_i( "import journal file '%s'", l_basename.c_str() );
        _applyChangeSet( m_things, ChangeSet( l_filename ) );

        // todo - should go to ChangeSet
        m_things.m_read_journals.insert(l_basename);
    }

    /// we return the actual changes to make them available for
    /// conflict solving

    ChangeSet l_result;

    if( !l_journalsToImport.empty() )
    {
        l_result = _diff( m_things_synced, m_things );
        deepCopy(m_things, m_things_synced);
    }
    return l_result;
}

ChangeSet zm::MindMatterModel::_persistence_pushJournal()
{
    ChangeSet l_changeSet( _diff( m_things_synced, m_things ) );

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

    _saveModel(m_things_synced, m_localModelFileSynced);

    return l_changeSet;
}

void zm::MindMatterModel::applyChangeSet(
        const ChangeSet    &changeSet )
{
    _applyChangeSet(m_things, changeSet);
}

void zm::MindMatterModel::_applyChangeSet(
        ModelData       &a_thingsMap,
        const ChangeSet &a_changeSet)
{
    for( const journal_ptr_t &j: a_changeSet.getJournal() )
    {
        const zm::uid_t &l_item_uid(j->item_uid);
        const zm::JournalItem::ChangeType &l_change_type(j->type);

        ModelData::left_iterator l_item_it( a_thingsMap.left.find( l_item_uid ) );

        if( l_change_type == JournalItem::CreateItem && l_item_it != a_thingsMap.left.end() )
        {
            trace_w( "trying to create already existent item '%s'",
                          l_item_uid.c_str() );
            trace_assert_s( l_change_type == JournalItem::CreateItem && l_item_it != a_thingsMap.left.end() );
            continue;
        }

        if( l_change_type != JournalItem::CreateItem && l_item_it == a_thingsMap.left.end() )
        {
            trace_w( "trying to modify non existent item '%s'",
                          l_item_uid.c_str() );
            trace_assert_s( j->type == JournalItem::CreateItem || l_item_it != a_thingsMap.left.end() );
            continue;
        }

        switch( l_change_type )
        {
        case JournalItem::CreateItem:
            _createNewItem(a_thingsMap, j->item_uid, j->value, j->time );
            break;
        case JournalItem::SetStringValue:
            _setValue(l_item_it, j->key, j->value );
            break;
        case JournalItem::EraseItem:
            _eraseItem(a_thingsMap, l_item_it );
            break;
        case JournalItem::ChangeCaption:
            _setCaption(l_item_it, j->value );
            break;
        case JournalItem::Connect:
        {
            ModelData::left_iterator l_item2_it(
                        a_thingsMap.left.find( j->key ) );
            trace_assert_h( l_item2_it != a_thingsMap.left.end() &&
                    "item to connect must exist");
            //_connectDuplex( l_item_it, l_item2_it, Directed );
            _connectSingle( l_item_it, l_item2_it, atoi(j->value.c_str()));
        } break;
        case JournalItem::Disconnect:
        {
            ModelData::left_iterator l_item2_it(
                        a_thingsMap.left.find( j->value ) );
            if(l_item2_it != a_thingsMap.left.end())
            {
                _disconnect( l_item_it, l_item2_it );
            }
            else
            {
                trace_w("trying to disconnect %s from %s",
                        l_item_uid.c_str(), j->value.c_str());
                trace_assert_s( l_item2_it != a_thingsMap.left.end() &&
                        "item to disconnect from must exist");
            }
        } break;
        case JournalItem::AddAttribute:
        {
            _addTag(a_thingsMap, l_item_it, j->key);
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
        trace_d( "%s", i->path().string().c_str() );
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

    trace_assert_h( l_fout.is_open() );

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
        trace_e("writing failed");
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
        trace_assert_h( !(n["uid"]) != !(n["read"]) );

        if( n["uid"] )
        {
            trace_assert_h( n["caption"] );

            uid_t l_uid = n.Tag();

            if( n["uid"] )
            {
                l_uid = n["uid"].as< uid_t >();
            }

            trace_assert_h( l_uid != "" && l_uid != "?" );

            std::string l_caption = n["caption"].as< std::string >();

            trace_d("caption: '%s'", l_caption.c_str());

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
                        trace_d("%s", l_conn.c_str());
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
                    trace_d("value '%s': '%s'", a.first.c_str(), a.second.c_str());
                }
            }

            if( n["hash1"] )
            {
                l_hashes[l_uid] = n["hash1"].as< std::string >();
            }
            trace_assert_h( l_new_thing->hasValue("global_time_created")
                    || l_uid == l_new_thing->m_caption );

            a_thingsMap.insert(
                        zm::ModelData::value_type(
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
    for(const zm::ModelData::value_type &i: a_thingsMap)
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
                zm::ModelData::left_iterator
                        l_other_it( a_thingsMap.left.find( other_uid.first ) );

                trace_assert_h(l_other_it != a_thingsMap.left.end());

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

            trace_assert_h(l_hash_it != l_hashes.end());

            if(l_hash_it->second != l_item->createHash())
            {
                trace_e("saved and loaded hashes differ!");
                trace_e("'%s' != '%s'",
                             l_hash_it->second.c_str(),
                             l_item->createHash(true).c_str());
                trace_assert_h(l_hash_it->second == l_item->createHash());
            }
        }
    }

    // _debug_dump(thingsMap);
}

/// for convenience: see persistence_saveLocalModel()
void zm::MindMatterModel::saveLocal()
{
    return persistence_saveLocalModel();
}

/// for convenience: see persistence_loadLocalModel()
bool zm::MindMatterModel::loadLocal()
{
    return persistence_loadLocalModel();
}

/// for convenience: see persistence_loadSnapshot()
bool zm::MindMatterModel::loadSnapshot()
{
    return persistence_loadSnapshot();
}

/// for convenience: see persistence_createSnapshot()
bool zm::MindMatterModel::createSnapshot()
{
    return persistence_createSnapshot();
}
