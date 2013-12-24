/// -*- coding: utf-8 -*-
///
/// file: testing.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#pragma once
#ifndef YSBOX_TESTING_H
#define YSBOX_TESTING_H

#include <string>
#include <map>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
#include <assert.h>

#define test_assert( cond, desc ) if(!(cond)){ \
    std::cout << "subtest '" << desc << "' failed!"; \
    assert((cond) && desc ); return false;}

#define TEST_RETURN_OK     0
#define TEST_RETURN_FAILED 1
#define TEST_RETURN_ERROR  2


/// for convenience - this way a test can use the test executable file name
char **g_arg_v;

typedef bool (*test_function)(void);
typedef std::pair< std::string, test_function > named_function;
typedef std::map< std::string, test_function > named_function_container;

inline bool run_single_test( named_function named_test )
{
    bool l_return = named_test.second();
/*
    std::cout << "running test: '" << named_test.first << "'.. ";

    if( l_return )
    {
        std::cout << "fine!" << std::endl;
    }
    else
    {
        std::cout << "FAILED!" << std::endl;
        std::cerr << "test failed: '" << named_test.first << "'" << std::endl;
    }
*/
    return l_return;
}

inline bool run_single_test( named_function_container::iterator named_test )
{
    return run_single_test( *named_test );
}

inline int run_tests( named_function_container &tests, int arg_n, char **arg_v )
{
    g_arg_v = arg_v;

    if( arg_n > 1 )
    {
        std::string l_test_identifier( arg_v[1] );

        if( tests.find( l_test_identifier ) == tests.end() )
        {
            std::cerr << "unknown test specifier: '" << l_test_identifier << "'" << std::endl;
            return TEST_RETURN_ERROR;
        }

        return run_single_test( tests.find( l_test_identifier ) ) ?
                    TEST_RETURN_OK : TEST_RETURN_FAILED;
    }
    else
    {
        bool l_all_passed(true);

        BOOST_FOREACH( named_function me, tests )
        {
            l_all_passed &= run_single_test( me );
        }
        return l_all_passed ?
                    TEST_RETURN_OK : TEST_RETURN_FAILED;
    }
}

#endif
