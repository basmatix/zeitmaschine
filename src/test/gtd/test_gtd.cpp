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

#include <iostream>
#include <fstream>

#include "../testing.h"

bool basic_workflow();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["basic_workflow"] = basic_workflow;

    return run_tests( l_tests, arg_n, arg_v );
}


bool basic_workflow()
{
    zmGtdModel l_m1;

    l_m1.setLocalFolder( "./test-localfolder" );

    l_m1.setUsedUsername( "test-user" );
    l_m1.setUsedHostname( "test-machine" );

    l_m1.initialize();

    test_assert( l_m1.empty(),
                 "nodes should be empty for the test" );

    std::string l_node1 = l_m1.createNewInboxItem( "urlaub planen" );

    l_m1.castToProject( l_node1 );


    std::string l_node2 = l_m1.createNewInboxItem( "gemeinsames Datum finden" );
    l_m1.registerItemAsTask( l_node2, l_node1 );

    std::string l_node3 = l_m1.createNewInboxItem( "Urlaubsort bestimmen" );
    l_m1.registerItemAsTask( l_node2, l_node1 );

    l_m1.setNextTask( l_node1, l_node3 );
    return true;
}
