/// -*- coding: utf-8 -*-
///
/// file: test_common.cpp
///
/// Copyright 2011 - 2013 scitics GmbH
///
/// Information  contained  herein  is  subject  to change  without  notice.
/// scitics GmbH  retains ownership and  all other rights  in this software.
/// Any reproduction of the software or components thereof without the prior
/// written permission of scitics GmbH is prohibited.

#include <mm/zmModel.h>

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <fstream>

#include "../testing.h"

bool change_while_open();
bool empty_db_on_load();
bool low_level_gtd_workflow();
bool connections();
bool diff_and_reapply();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["change_while_open"] = change_while_open;
    l_tests["empty_db_on_load"] = empty_db_on_load;
    l_tests["low_level_gtd_workflow"] = low_level_gtd_workflow;
    l_tests["connections"] = connections;

    return run_tests( l_tests, arg_n, arg_v );
}


bool connections()
{
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

bool diff_and_reapply()
{
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
    // save
    //
    l_m1.localSave();

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


    // modify

    // diff
    zm::ChangeSet c = l_m2.diff( l_m1 );

    // apply
    l_m2.applyChangeSet( c );

    // compare
    test_assert( l_m2 == l_m1,
                 "both model should hold same data after applying changeset" );


    return true;
}

bool empty_db_on_load()
{
    zm::MindMatterModel l_m1;

    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );
    l_m1.setLocalFolder( "./test-localfolder" );
    l_m1.initialize();

    /*
    // client 1 saves some content
    l_m1.setLocalFolder( "zmtest" );
    std::string l_item1 = l_m1.createNewItem( "some first item" );
    l_m1.localSave();
    //l_m1.save( "tmp_export.yaml" );

    MindMatterModel l_m2;
    l_m1.setLocalFolder( "zmtest" );
    std::string l_item2 = l_m2.createNewItem( "yet some item" );
    l_m1.localSave();

    //l_m2.load( "tmp_export.yaml" );

    bool l_test_passed = l_m2.hasItem( l_item1 ) && ! l_m2.hasItem( l_item2 );
    return l_test_passed;
*/
    test_assert( false, "dummy test should not pass" );

    return true;
}

bool change_while_open()
{
    // client 1 starts and has a model in mind
    zm::MindMatterModel l_m1;
    std::string l_item1 = l_m1.createNewItem( "some first item" );

    // meanwhile another client (or some syncing system) writes some content
    zm::MindMatterModel l_m2;
    std::string l_item2 = l_m2.createNewItem( "some concurrent item" );
    //l_m2.save( "tmp_export.yaml" );


    // now the first model decides to dump it's mind
    //l_m1.save( "tmp_export.yaml" );

    // client 2 (or a third client) starts again and loads the current db
    zm::MindMatterModel l_m3;
//    std::cout <<  boost_concept_check__LINE__ << std::endl;

    bool l_everythings_there =
            l_m3.hasItem( l_item1 )  && l_m3.hasItem( l_item2 );

    return l_everythings_there;
}


bool low_level_gtd_workflow()
{
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
    l_m1.connect( l_gtd_item1, l_item_inbox );


    /// decide this item to be a project
    l_m1.disconnect( l_gtd_item1, l_item_inbox );
    l_m1.connect( l_gtd_item1, l_item_project );


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

