/// -*- coding: utf-8 -*-
///
/// file: test_common.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///


#include "../testing.h"

#include <mm/zmOptions.h>

#include <boost/filesystem/operations.hpp>

bool common_options_basic();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["common_options_basic"] = common_options_basic;

    return run_tests( l_tests, arg_n, arg_v );
}

bool common_options_basic()
{
    zm::zmOptions o1;

    boost::filesystem::remove( "test.json" );

    o1.load( "test.json" );

    test_assert( o1.hasValue( "first_value" ) == false,
                 "value should be non existent before setting" );

    o1.setInt( "first_value", 6 );

    test_assert( o1.getInt( "first_value" ) == 6,
                 "red value should be the one we set" );

    test_assert( o1.hasValue( "first_value" ),
                 "value should exist after setting" );

    o1.setInt( "group.first_value", 7 );

    test_assert( o1.getInt( "group.first_value" ) == 7,
                 "red value should be the one we set" );

    std::vector< std::string > v;
    v.push_back("hallo");
    v.push_back("welt");

    /*
    o1.addStringListElement("group.strings",v[0]);
    o1.addStringListElement("group.strings",v[1]);

    test_assert( o1.getStringList( "group.strings" ) == v,
                 "returning string list should be the one we provided" );
    */
    zm::zmOptions o2;

    test_assert( o2.hasValue( "first_value" ) == false,
                 "value should be non existent before loading" );

    o2.load( "test.json" );

    test_assert( o2.hasValue( "first_value" ),
                 "value should exist after loading" );

    test_assert( o2.getInt( "first_value" ) == 6,
                 "red value should be the one we set for o1 before" );

    return true;
}
