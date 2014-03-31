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

#include <zm/zmGtdModel.h>

#include <testing/testing.h>
#include <testing/common.h>

#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>

bool gtd_basic_workflow ();
bool gtd_filter_tags    ();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["gtd_basic_workflow"] = gtd_basic_workflow;
    l_tests["gtd_filter_tags"]    = gtd_filter_tags;

    return run_tests( l_tests, arg_n, arg_v );
}

bool gtd_basic_workflow()
{
    CleanFolder fc1("./test-localfolder");

    zmGtdModel l_m1(zm::MindMatterModel::create());

    l_m1.base()->setLocalFolder( fc1 );
    l_m1.base()->setUsedUsername( "test-user" );
    l_m1.base()->setUsedHostname( "test-machine" );
    l_m1.initialize();

    test_assert( l_m1.empty(),
                 "nodes should be empty for the test" );

    std::string l_node1 = l_m1.createNewInboxItem( "urlaub planen" );

    l_m1.castToProject( l_node1 );


    std::string l_node2 = l_m1.createNewInboxItem( "gemeinsames Datum finden" );
    l_m1.registerItemAsTask( l_node2, l_node1 );

    std::string l_node3 = l_m1.createNewInboxItem( "Urlaubsort bestimmen" );
    l_m1.registerItemAsTask( l_node3, l_node1 );

    l_m1.setNextTask( l_node1, l_node3 );
    return true;
}

bool gtd_filter_tags()
{
    CleanFolder l_cf("./test-localfolder" );
    zmGtdModel l_m(zm::MindMatterModel::create());
    l_m.base()->setLocalFolder( l_cf );
    l_m.base()->setUsedUsername( "test-user" );
    l_m.base()->setUsedHostname( "test-machine" );
    l_m.initialize();

    zm::uid_t l_p1 = l_m.createNewInboxItem("job finden");
    l_m.castToProject( l_p1 );

    zm::uid_t l_t1 = l_m.createNewInboxItem( "Anforderungen definieren" );
    l_m.registerItemAsTask( l_t1, l_p1 );

    zm::uid_lst_t l_l1 = l_m.getInboxItems(false);
    zm::uid_lst_t l_l2 = l_m.getTaskItems(false, false);
    zm::uid_lst_t l_l3 = l_m.getProjectItems(false, false);
    zm::uid_lst_t l_l4 = l_m.getDoneItems();

    test_assert(l_l1.size() == 0, "expected number of inbox item is correct");
    test_assert(l_l2.size() == 1, "expected number of inbox item is correct");
    test_assert(l_l3.size() == 1, "expected number of inbox item is correct");
    test_assert(l_l4.size() == 0, "expected number of inbox item is correct");

    return true;
}
