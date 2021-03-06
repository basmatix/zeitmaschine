/// -*- coding: utf-8 -*-
///
/// file: test_mindmatter.cpp
///
/// Copyright (C) 2013 Frans Fuerst


#include <testing/common.h>
#include <testing/testing.h>

#include <mm/zmModel.h>
#include <mm/zmTrace.h>

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <fstream>

bool mm_change_while_open       ();
bool mm_empty_db_on_load        ();
bool mm_low_level_gtd_workflow  ();
bool mm_connections             ();
bool mm_diff_and_reapply        ();
bool mm_persist_and_load        ();
bool mm_journaled_sync          ();
bool mm_equality                ();
bool mm_snapshot                ();
bool mm_subfolders              ();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    set_trace_level(4);

    // l_tests["mm_change_while_open"] =       mm_change_while_open;
    l_tests["mm_equality"] =                mm_equality;
    l_tests["mm_connections"] =             mm_connections;
    l_tests["mm_low_level_gtd_workflow"] =  mm_low_level_gtd_workflow;
    l_tests["mm_empty_db_on_load"] =        mm_empty_db_on_load;
    l_tests["mm_diff_and_reapply"] =        mm_diff_and_reapply;
    l_tests["mm_persist_and_load"] =        mm_persist_and_load;
    l_tests["mm_journaled_sync"] =          mm_journaled_sync;
    l_tests["mm_snapshot"] =                mm_snapshot;
    l_tests["mm_subfolders"] =              mm_subfolders;

    return run_tests( l_tests, arg_n, arg_v );
}

bool mm_equality()
{
    CleanFolder fc1("./test-localfolder-1");

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( fc1 );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();

    test_assert(l_m1.createHash() == l_m2.createHash(),
                "hashes of empty models should be equal");

    std::string node1 = l_m1.createNewItem( "node1" );
    std::string node2 = l_m1.createNewItem( "node2" );
    std::string node3 = l_m1.createNewItem( "node3" );
    l_m1.connectDirected(node2, node3);

    test_assert(l_m1.createHash() != l_m2.createHash(),
                "hashes of different models should differ");

    l_m1.duplicateModelTo(l_m2);

    l_m1.debug_dump();

    l_m2.debug_dump();

    test_assert(l_m1.createHash() == l_m2.createHash(),
                "hashes of synced models should be equal");

    return true;
}

bool mm_journaled_sync()
{
    CleanFolder fc1("./test-localfolder-1");
    CleanFolder fc2("./test-localfolder-2");

    int l_files_copied;
    bool l_synced;

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( fc2 );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();


    ///
    /// modify model1
    ///
    zm::uid_t node1 = l_m1.createNewItem( "node1" );

    /// save and re-initialize. this addresses a bug which leads to
    /// unability to sync after altering a (new) model and restarting
    l_m1.saveLocal();
    l_m1.initialize();


    ///
    /// sync 1  (first model1 then model2)
    ///
    l_synced = l_m1.sync_push();
    l_files_copied = sync_folders(fc1, fc2);
    test_assert(l_files_copied == 1, "exactly one file should have been copied");

    l_synced = l_m2.sync_pull();
    test_assert(l_m2.equals(l_m1), "model2 should not differ from model1");


    ///
    /// modify model1
    ///
    zm::uid_t node2 = l_m1.createNewItem( "node2" );
    zm::uid_t node3 = l_m1.createNewItem( "node3" );
    l_m1.connectDirected(node2, node3);


    ///
    /// sync 2  (first model1 then model2)
    ///

    l_synced = l_m1.sync_push();
    test_assert( l_synced, "a sync file should have been generated");

    l_synced = l_m1.sync_push();
    l_m1.saveLocal();
    test_assert( ! l_synced, "second sync should have no effect");

    l_files_copied = sync_folders(fc1, fc2);
    test_assert(l_files_copied == 1, "exactly one file should have been copied")

    l_synced = l_m2.sync_push();
    test_assert( ! l_synced, "push should not succeed due to remote changes");

    l_synced = l_m2.sync_pull();
    test_assert( l_synced, "changes should have been pulled");

    l_synced = l_m2.sync_pull();
    test_assert( ! l_synced, "second pull should have no effect");

    test_assert( l_m2.equals(l_m1), "model2 should not differ from model1");

    l_m1.initialize();

    test_assert( ! l_m1.hasLocalChanges(), "should have no changes after sync");

    return true;
}

bool mm_persist_and_load()
{
    /// NOTE: in this example two models act on the same folder which
    ///       is not intended and will be forbidden soon.
    ///       dont take this as an example
    CleanFolder fc1("./test-localfolder");

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( fc1 );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();

    std::string node1 = l_m1.createNewItem( "node1" );
    std::string node2 = l_m1.createNewItem( "node2" );

    test_assert( ! l_m2.equals(l_m1), "model2 should differ from model1");
    l_m1.persistence_saveLocalModel();
    l_m2.persistence_loadLocalModel();
    test_assert( l_m2.equals(l_m1), "model2 should not differ from model1");

    std::string node3 = l_m1.createNewItem( "node3" );

    l_m1.connectDirected(node1, node3);

    test_assert( ! l_m2.equals(l_m1), "model2 should differ from model1");
    printf("save..\n"); fflush(stdout);
    l_m1.persistence_saveLocalModel();
    printf("load..\n"); fflush(stdout);
    l_m2.persistence_loadLocalModel();
    test_assert( l_m2.equals(l_m1), "model2 should not differ from model1");

    l_m1.connectDirected(node1, node2);
    test_assert( ! l_m2.equals(l_m1), "model2 should differ from model1");


    /// this piece of code just tests whether it's possible to load
    /// a model which differs from its last synced state
    zm::MindMatterModel l_m3;
    l_m3.setLocalFolder( fc1 );
    l_m3.setUsedUsername( "test-user" );
    l_m3.setUsedHostname( "test-machine" );
    l_m3.initialize();
    l_m3.sync_pull();

    return true;
}

bool mm_connections()
{
    CleanFolder fc1("./test-localfolder");

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.getItemCount() == 0,
                 "nodes should be empty for the test" );

    std::string node1 = l_m1.createNewItem( "node1" );
    std::string node2 = l_m1.createNewItem( "node2" );
    std::string node3 = l_m1.createNewItem( "node3" );

    l_m1.connectDirected( node1, node2 );

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
    /// NOTE: in this example two models act on the same folder which
    ///       is not intended and will be forbidden soon.
    ///       dont take this as an example

    CleanFolder fc1("./test-localfolder");

    // [todo] - test circular dependencies like
    //          [new item 1] <=> [new item 2]
    // [todo] - test new connections
    // [todo] - test deleted connections
    // [todo] - test tags across different 'devices'
    // [todo] - test deletion of string values
    // [todo] - remove connections
    // [todo] - change connection type


    //
    // create first model
    //
    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
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
    l_m1.persistence_saveLocalModel();

    //
    // fork
    //
    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( fc1 );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();

    test_assert( l_m2.equals( l_m1 ),
                 "both model should hold same data" );


    // make some modification on m1

    std::string item1 = l_m1.createNewItem("some new item");

    test_assert( not l_m2.equals( l_m1 ), "models should differ from each other" );

    // generate a change set

    zm::ChangeSet l_changeset = l_m2.diffTo( l_m1 );

    // and apply it
    l_m2.applyChangeSet( l_changeset );

    // compare again
    test_assert( l_m2.equals( l_m1, true ),
                 "both model should hold same data after applying changeset" );

    // some more modification
    std::string item2 = l_m1.createNewItem("yet some new item");

    l_m1.connectDirected(item1, item2);

    test_assert( not l_m2.equals( l_m1 ), "models should differ from each other" );

    // generate a change set
    l_m1.debug_dump();
    l_m2.debug_dump();
    l_changeset = l_m2.diffTo( l_m1 );
    l_changeset.debug_dump();

    // apply
    l_m2.applyChangeSet( l_changeset );

    l_m1.debug_dump();
    l_m2.debug_dump();

    // compare
    test_assert( l_m2.equals( l_m1, true ),
                 "both model should hold same data after applying changeset" );

    return true;
}

bool mm_empty_db_on_load()
{
    /// NOTE: in this example two models act on the same folder which
    ///       is not intended and will be forbidden soon.
    ///       dont take this as an example

    CleanFolder fc1("./test-localfolder");

    zm::MindMatterModel l_m1;
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.setLocalFolder( fc1 );
    l_m1.initialize();

    // client 1 saves some content
    std::string l_item1 = l_m1.createNewItem( "some first item" );
    l_m1.persistence_saveLocalModel();
    //l_m1.save( "tmp_export.yaml" );

    zm::MindMatterModel l_m2;
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.setLocalFolder( fc1 );
    l_m2.initialize();

    std::string l_item2 = l_m1.createNewItem( "yet some item" );
    l_m1.persistence_saveLocalModel();

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

    CleanFolder fc1("./test-localfolder");

    // client 1 starts and has a model in mind
    zm::MindMatterModel l_m1;
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.setLocalFolder( fc1 );
    l_m1.initialize();

    std::string l_item1 = l_m1.createNewItem( "some first item" );

    // meanwhile another client (or some syncing system) writes some content
    zm::MindMatterModel l_m2;
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.setLocalFolder( fc1 );
    l_m2.initialize();

    std::string l_item2 = l_m2.createNewItem( "some concurrent item" );

    l_m2.persistence_saveLocalModel();

    // now the first model decides to dump it's mind
    l_m1.sync_push();

    // client 2 (or a third client) starts again and loads the current db
    zm::MindMatterModel l_m3;

    test_assert( l_m3.hasItem( l_item1 ), "" );

    test_assert( l_m3.hasItem( l_item2 ), "" );

    return true;
}


bool mm_low_level_gtd_workflow()
{
    CleanFolder fc1("./test-localfolder");

    zm::MindMatterModel l_m1;

    l_m1.setLocalFolder( fc1 );
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
    l_m1.connectDirected( l_gtd_item2, l_item_inbox );

    std::string l_gtd_item3 = l_m1.createNewItem("Urlaubsort bestimmen");
    l_m1.connectDirected( l_gtd_item3, l_item_inbox );


    /// make them tasks and connect them with task1
    l_m1.connectDirected( l_gtd_item2, l_item_task );
    l_m1.disconnect( l_gtd_item2, l_item_inbox );
    l_m1.connectDirected( l_gtd_item2, l_gtd_item1 );

    l_m1.connectDirected( l_gtd_item3, l_item_task );
    l_m1.disconnect( l_gtd_item3, l_item_inbox );
    l_m1.connectDirected( l_gtd_item3, l_gtd_item1 );


    /// decide item3 to be the first step to take
    l_m1.connectDirected( l_gtd_item3, l_item_next_task );

    return true;
}

bool mm_snapshot()
{
    /// a snapshot ensures there is a "starting" model to create the
    /// local model from
    /// it's important to ensure that models created from the last
    /// snapshot contain exactly the same information as the source
    /// model and those which have been synced incrementally

    CleanFolder fc1("./test-localfolder-1");
    CleanFolder fc2("./test-localfolder-2");
    CleanFolder fc3("./test-localfolder-3");

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    zm::uid_t l_item_inbox = l_m1.findOrCreateTagItem( "huhu" );
    zm::uid_t l_item2 = l_m1.createNewItem( "hallo" );

    bool l_synced;
    bool l_files_copied;

    l_synced = l_m1.sync_push();
    test_assert( l_synced, "files should have been synced" );

    zm::uid_t l_item3 = l_m1.createNewItem( "du" );
    l_m1.connectDirected(l_item2, l_item3);

    l_synced = l_m1.sync_push();

    l_files_copied = sync_folders(fc1, fc2);
    test_assert( l_synced, "files should have been synced" );

    zm::MindMatterModel l_m2;
    l_m2.setLocalFolder( fc2 );
    l_m2.setUsedUsername( "test-user" );
    l_m2.setUsedHostname( "test-machine" );
    l_m2.initialize();

    l_synced = l_m2.sync_pull();

    test_assert( l_m1.equals( l_m2, true), "models should be equal" );
    test_assert( l_m2.equals( l_m1, true), "models should be equal" );

    l_m2.persistence_createSnapshot();

    l_files_copied = sync_folders(fc2, fc3, "snapshot-*.yaml");
    test_assert( l_synced, "files should have been synced" );

    zm::MindMatterModel l_m3;
    l_m3.setLocalFolder( fc3 );
    l_m3.setUsedUsername( "test-user" );
    l_m3.setUsedHostname( "test-machine" );
    l_m3.initialize();

    test_assert( l_m3.equals( l_m1, true), "models should be equal" );

    return true;
}

bool mm_subfolders()
{
    CleanFolder fc1("./test-localfolder");

    zm::MindMatterModel l_m1;
    l_m1.setLocalFolder( fc1 );
    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.getItemCount() == 0,
                 "nodes should be empty for the test" );

    std::string node1 = l_m1.createNewItem( "node1" );

    std::string node2 = l_m1.createNewItem( "folder1" );

    l_m1.putIntoFolder(node1, node2);

    zm::uid_lst_t l( {"eins","zwei"} );

    zm::uid_lst_t l_return = l_m1.getFolderChildren(node2);

    test_assert(l_return == zm::uid_lst_t({"eins","zwei"}),"bla");
    // todo subfolders
    return true;
}
