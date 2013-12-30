/// -*- coding: utf-8 -*-
///
/// file: test_mindmatter.cpp
///
/// Copyright (C) 2013 Frans Fuerst

#include <mm/zmModel.h>

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>

#include "../testing.h"

bool mm_change_while_open       ();
bool mm_empty_db_on_load        ();
bool mm_low_level_gtd_workflow  ();
bool mm_connections             ();
bool mm_diff_and_reapply        ();
bool mm_persist_and_load        ();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    // l_tests["mm_change_while_open"] =       mm_change_while_open;
    l_tests["mm_empty_db_on_load"] =        mm_empty_db_on_load;
    l_tests["mm_low_level_gtd_workflow"] =  mm_low_level_gtd_workflow;
    l_tests["mm_connections"] =             mm_connections;
    l_tests["mm_diff_and_reapply"] =        mm_diff_and_reapply;
    l_tests["mm_persist_and_load"] =        mm_persist_and_load;

    return run_tests( l_tests, arg_n, arg_v );
}

bool mm_persist_and_load()
{
    return true;
}

bool mm_connections()
{
    if( boost::filesystem::exists( "./test-localfolder" ) )
        boost::filesystem::remove_all( "./test-localfolder" );

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.getItemCount() == 0,
                 "nodes should be empty for the test" );

    std::string node1 = l_m1.createNewItem( "node1" );
    std::string node2 = l_m1.createNewItem( "node2" );
    std::string node3 = l_m1.createNewItem( "node3" );

    l_m1.connect( node1, node2 );

    test_assert( l_m1.isConnected(node1, node2),
                 "node1 and node2 should be connected after connection" );

    test_assert( l_m1.isConnected(node2, node1),
                 "node1 and node2 should be connected after connection" );

    test_assert( !l_m1.isConnected(node1, node3),
                 "node1 and node2 should be connected after connection" );

    return true;
}

bool mm_diff_and_reapply()
{
    if( boost::filesystem::exists( "./test-localfolder" ) )
        boost::filesystem::remove_all( "./test-localfolder" );

    // [todo] - test circular dependencies like
    //          [new item 1] <=> [new item 2]
    // [todo] - test new connections
    // [todo] - test deleted connections
    // [todo] - test tags across different 'devices'
    // [todo] - test deletion of string values


    //
    // create first model
    //
    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.getItemCount() == 0,
                 "nodes should be empty for the test" );

    //
    // generate some content for m1
    //
    // TBD

    //
    // save m1
    //
    l_m1.persistence_localSave();

    //
    // fork
    //
    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( "./test-localfolder" );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();

    test_assert( l_m2 == l_m1,
                 "both model should hold same data" );


    // make some modification on m1

    std::string item1 = l_m1.createNewItem("some new item");

    test_assert( l_m2 != l_m1, "models should differ from each other" );

    // generate a change set

    zm::ChangeSet l_changeset = l_m2.diffTo( l_m1 );

    // and apply it
    l_m2.applyChangeSet( l_changeset );

    // compare again
    test_assert( l_m2.equals( l_m1, true ),
                 "both model should hold same data after applying changeset" );

    // some more modification
    std::string item2 = l_m1.createNewItem("yet some new item");

    l_m1.connect(item1, item2);

    test_assert( l_m2 != l_m1, "models should differ from each other" );

    // generate a change set
    l_changeset = l_m2.diffTo( l_m1 );

    // apply
    l_m2.debug_dump();
    l_m2.applyChangeSet( l_changeset );

    // compare
    test_assert( l_m2.equals( l_m1, true ),
                 "both model should hold same data after applying changeset" );


    return true;
}

bool mm_empty_db_on_load()
{
    if( boost::filesystem::exists( "./test-localfolder" ) )
        boost::filesystem::remove_all( "./test-localfolder" );

    zm::MindMatterModel l_m1;
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.initialize();

    // client 1 saves some content
    std::string l_item1 = l_m1.createNewItem( "some first item" );
    l_m1.persistence_localSave();
    //l_m1.save( "tmp_export.yaml" );

    zm::MindMatterModel l_m2;
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.setLocalFolder( "./test-localfolder" );
    l_m2.initialize();

    std::string l_item2 = l_m1.createNewItem( "yet some item" );
    l_m1.persistence_localSave();

    //l_m2.load( "tmp_export.yaml" );

    bool b1 = l_m2.hasItem( l_item1 );
    bool b2 = l_m2.hasItem( l_item2 );

    test_assert( b1 && ! b2,
                 "m2 should contain item1 but not item2");

    return true;
}

bool mm_change_while_open()
{   /// this test is not more valid in case one local folder is being
    /// used by one client (e.g. a local server) at a time.
    /// thus it will likely fail forever

    if( boost::filesystem::exists( "./test-localfolder" ) )
        boost::filesystem::remove_all( "./test-localfolder" );

    // client 1 starts and has a model in mind
    zm::MindMatterModel l_m1;
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.initialize();

    std::string l_item1 = l_m1.createNewItem( "some first item" );

    // meanwhile another client (or some syncing system) writes some content
    zm::MindMatterModel l_m2;
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.setLocalFolder( "./test-localfolder" );
    l_m2.initialize();

    std::string l_item2 = l_m2.createNewItem( "some concurrent item" );

    l_m2.persistence_localSave();

    // now the first model decides to dump it's mind
    l_m1.persistence_sync();

    // client 2 (or a third client) starts again and loads the current db
    zm::MindMatterModel l_m3;

    test_assert( l_m3.hasItem( l_item1 ), "" );

    test_assert( l_m3.hasItem( l_item2 ), "" );

    return true;
}


bool mm_low_level_gtd_workflow()
{
    if( boost::filesystem::exists( "./test-localfolder" ) )
        boost::filesystem::remove_all( "./test-localfolder" );

    zm::MindMatterModel l_m1;

    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.getItemCount() == 0,
                 "nodes should be empty for the test" );

    std::string l_item_inbox =      l_m1.findOrCreateTagItem( "gtd_inbox" );
    std::string l_item_task =       l_m1.findOrCreateTagItem( "gtd_task" );
    std::string l_item_next_task =  l_m1.findOrCreateTagItem( "gtd_next_task" );
    std::string l_item_project =    l_m1.findOrCreateTagItem( "gtd_project" );
    std::string l_item_group =      l_m1.findOrCreateTagItem( "gtd_group" );
    std::string l_item_done =       l_m1.findOrCreateTagItem( "gtd_done" );
    std::string l_item_knowledge =  l_m1.findOrCreateTagItem( "knowledge" );
    std::string l_item_person =     l_m1.findOrCreateTagItem( "person" );

    /// create new gtd item
    std::string l_gtd_item1 = l_m1.createNewItem("urlaub planen");
    l_m1.addTag( l_gtd_item1, "gtd_inbox" );
    test_assert( l_m1.isConnected( l_gtd_item1, l_item_inbox ),
                 "node properly connected to tag node" );

    /// decide this item to be a project
    l_m1.removeTag( l_gtd_item1, "gtd_inbox" );
    l_m1.addTag( l_gtd_item1, "gtd_project" );

    test_assert( !l_m1.isConnected( l_gtd_item1, l_item_inbox ),
                 "node properly connected to tag node" );
    test_assert( l_m1.isConnected( l_gtd_item1, l_item_project ),
                 "node properly disconnected from tag node" );


    /// find two subtasks
    std::string l_gtd_item2 = l_m1.createNewItem("gemeinsames Datum finden");
    l_m1.connect( l_gtd_item2, l_item_inbox );

    std::string l_gtd_item3 = l_m1.createNewItem("Urlaubsort bestimmen");
    l_m1.connect( l_gtd_item3, l_item_inbox );


    /// make them tasks and connect them with task1
    l_m1.connect( l_gtd_item2, l_item_task );
    l_m1.disconnect( l_gtd_item2, l_item_inbox );
    l_m1.connect( l_gtd_item2, l_gtd_item1 );

    l_m1.connect( l_gtd_item3, l_item_task );
    l_m1.disconnect( l_gtd_item3, l_item_inbox );
    l_m1.connect( l_gtd_item3, l_gtd_item1 );


    /// decide item3 to be the first step to take
    l_m1.connect( l_gtd_item3, l_item_next_task );

    return true;
}
