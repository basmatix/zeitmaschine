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
bool output_node();

int main( int arg_n, char **arg_v )
{
    named_function_container l_tests;

    l_tests["change_while_open"] = change_while_open;
    l_tests["empty_db_on_load"] = empty_db_on_load;
    l_tests["output_node"] = output_node;

    return run_tests( l_tests, arg_n, arg_v );
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

bool output_node()
{
    std::ofstream l_fout( "testfile.yaml" );
    YAML::Emitter out(l_fout);

    // main sequence
    out << YAML::BeginSeq;

        out << YAML::VerbatimTag("048d5cb69c066bea");

        out << YAML::BeginMap;
            out << YAML::Key << "StringValues";
            out << YAML::Value;
                out << YAML::BeginMap;
                    out << YAML::Key << "position";
                    out << YAML::Value << "3B";
                out << YAML::EndMap;
        out << YAML::EndMap;

        out << YAML::Key << "attributes";
        out << YAML::Value ;
        out << YAML::BeginSeq;
        out << "eins";
        out << "zwei";
        out << YAML::EndSeq;

    out << YAML::VerbatimTag("abcuniquedef2");

    out << YAML::EndSeq;
    l_fout << std::endl;

    return true;
}
