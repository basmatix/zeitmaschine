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


//#include <iostream>
//#include <fstream>

#include "../testing.h"

#include <mm/zmOptions.h>


//#include <boost/filesystem/operations.hpp>

bool options_basic();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["options_basic"] = options_basic;

    return run_tests( l_tests, arg_n, arg_v );
}


bool options_basic()
{

    return true;
}
