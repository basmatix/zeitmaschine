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
    , m_initialized             ( false )
    , m_options                 ( new zm::zmOptions )
    , m_checkHashes             ( true )
{
    trace_i("hostname: %s", zm::osal::getHostName().c_str());
    trace_i("username: %s", zm::osal::getUserName().c_str());
}

zm::MindMatterModel::~MindMatterModel()
{
    clear(m_things_synced);
    clear(m_things);
    delete m_options;
}

void zm::MindMatterModel::setTraceLevel(int level)
{
    set_trace_level(level);
}

void zm::MindMatterModel::setLocalFolder( const std::string &a_path )
{
    trace_i( "privateDir: %s", a_path.c_str() );

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

void zm::MindMatterModel::disableHashChecking()
{
    m_checkHashes = false;
}

void zm::MindMatterModel::initialize()
{
    /// setLocalFolder() has not been called yet. use the default folder
    if(m_localFolderRoot == "")
    {
        setLocalFolder( ( boost::filesystem::path(
                              zm::osal::getHomePath()) / "zm-local").string() );
    }
    if(not hasUsedUsername())
    {
        setUsedUsername(zm::osal::getUserName());
    }
    if(not hasUsedHostname())
    {
        setUsedHostname(zm::osal::getHostName());
    }

    srand( time( NULL ) * rand()  );

    /// find the name for the local model file - should be equal
    /// across sessions and unique for each client
    // <local folder>/zm-<user>-<client>-<zm-domain>-local.yaml
    // eg. /path/to/zeitmaschine/zm-frans-heizluefter-private-local.yaml



    trace_i("options: username: %s", m_options->getString("username").c_str());
    trace_i("options: hostname: %s", m_options->getString("hostname").c_str());

    m_localModelFile = createModelFileNameNew();

    m_localModelFileSynced = createModelFileNameOld();

    if( !persistence_loadLocalModel() )
    {
        // should only be done on syncing
        persistance_loadSnapshot();
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

bool zm::MindMatterModel::equals(
        const ModelData &a_first,
        const ModelData &a_second,
        bool tell_why )
{
    /// prune if models differ in size
    if( a_first.size() != a_second.size() )
    {
        if(tell_why)
        {
            trace_i("MindMatterModels have different model size" );
        }
        return false;
    }

    /// prune if models differ in size
    if( a_first.m_read_journals.size() != a_second.m_read_journals.size() )
    {
        if(tell_why)
        {
            trace_i("MindMatterModel have different journal history size");
        }
        return false;
    }

    /// go through all elements of m_things - note that we don't have
    /// to do this for the second model
    for( const ModelData::value_type& i: a_first )
    {
        /// find the key in the other map
        ModelData::left_const_iterator l_item_it(
                    a_second.left.find( i.left ) );

        /// not found? return false!
        if( l_item_it == a_second.left.end() )
        {
            if(tell_why)
            {
                trace_i("MindMatterModels differ: item %s not found in "
                             "second model",
                             i.left.c_str() );
            }
            return false;
        }

        /// values differ? return false!
        if( ! i.right->equals( *l_item_it->second, tell_why ) )
        {
            if(tell_why)
            {
                trace_i("items '%s' differ in both models", i.left.c_str() );
            }
            return false;
        }
    }

    if( a_first.m_read_journals != a_second.m_read_journals )
    {
        if(tell_why)
        {
            trace_i("MindMatterModels have different journal history");
        }
        return false;
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

    for( const ModelData::value_type& i: model_to )
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
        l_return.append( i.right->toJournal(l_other_item_id) );
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

void zm::MindMatterModel::addDomainSyncFolder(
        const std::string &domainName,
        const std::string &path )
{
    trace_i( "new domain: %s %s", domainName.c_str(), path.c_str() );
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

std::string zm::MindMatterModel::createSnapshotFileName() const
{
    std::stringstream l_ssFileName;
    l_ssFileName << m_localFolderSync << "/snapshot"
                 << "-"
                 << zm::common::time_stamp_iso()
                 << ".yaml";

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

const zm::MindMatterModel::ModelData & zm::MindMatterModel::things() const
{
    return m_things;
}

size_t zm::MindMatterModel::getItemCount() const
{
    return m_things.size();
}

std::vector< zm::uid_t > zm::MindMatterModel::getItems() const
{
    std::vector< zm::uid_t > l_result;
    for(const ModelData::value_type &i: m_things)
    {
        l_result.push_back(i.left);
    }

    return l_result;
}

void zm::MindMatterModel::clear( ModelData &thingsMap )
{
    BOOST_FOREACH(const ModelData::value_type& i, thingsMap)
    {
        delete i.right;
    }
    thingsMap.clear();
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

    for(const zm::MindMatterModel::ModelData::value_type &i: a_source)
    {
        MindMatter *l_new_thing = new MindMatter(i.right->m_caption);
        l_new_thing->m_string_values = i.right->m_string_values;

        a_dest.insert(
                    zm::MindMatterModel::ModelData::value_type(
                        i.left, l_new_thing ) );
    }

    for(const zm::MindMatterModel::ModelData::value_type &i: a_source)
    {
        ModelData::left_iterator l_dest_first_item_it(
                    a_dest.left.find( i.left ) );

        assert( l_dest_first_item_it != a_dest.left.end() );

        for(const auto & l_neighbour: i.right->m_neighbours)
        {
            ModelData::left_iterator l_dest_second_item_it(
                        a_dest.left.find( l_neighbour.second.first ) );

            assert( l_dest_second_item_it != a_dest.left.end() );

            _connectSingle(
                        l_dest_first_item_it,
                        l_dest_second_item_it,
                        l_neighbour.second.second);
        }
    }
    a_dest.m_read_journals = a_source.m_read_journals;
}

void zm::MindMatterModel::duplicateModelTo(MindMatterModel &other) const
{
    deepCopy(m_things, other.m_things);
    deepCopy(m_things, other.m_things_synced);
}

void zm::MindMatterModel::ModelData::debug_dump() const
{
    trace_i(">> dump");

    for(const zm::MindMatterModel::ModelData::value_type& i: m_data)
    {
        std::ostringstream l_neighbours;
        l_neighbours << "(";
        for( const item_neighbour_pair_t &m: i.right->m_neighbours)
        {
            l_neighbours << m.second.first << "(" << m.second.second << ")" << ", ";
        }
        l_neighbours << ")";
        trace_i("  %s (#%s) %s '%s'",
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
