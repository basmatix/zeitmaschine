#include <zmModel.h>

#include <iostream>

bool change_while_open();
bool empty_db_on_load();
bool output_node();

int main( int arg_n, char **arg_v )
{
    if( arg_n < 2 )
    {
        std::cerr << "no test specified" << std::endl;
        return 2;
    }

    std::string l_test_identifier( arg_v[1] );

    if( l_test_identifier == "empty-db-on-load" )
        return empty_db_on_load() ? 0 : 1;

    if( l_test_identifier == "change-while-open" )
        return change_while_open() ? 0 : 1;

    if( l_test_identifier == "output-node" )
        return output_node() ? 0 : 1;

    std::cerr << "unknown test specifier: '" << l_test_identifier << "'" << std::endl;
    return 3;
}

bool empty_db_on_load()
{
    // client 1 saves some content
    ThingsModel l_m1;
    std::string l_item1 = l_m1.createNewItem( "some first item" );
    l_m1.save( "tmp_export.yaml" );

    ThingsModel l_m2;
    std::string l_item2 = l_m2.createNewItem( "yet some item" );

    l_m2.load( "tmp_export.yaml" );

    bool l_test_passed = l_m2.hasItem( l_item1 ) && ! l_m2.hasItem( l_item2 );
    return l_test_passed;
}

bool change_while_open()
{
    // client 1 starts and has a model in mind
    ThingsModel l_m1;
    std::string l_item1 = l_m1.createNewItem( "some first item" );

    // meanwhile another client (or some syncing system) writes some content
    ThingsModel l_m2;
    std::string l_item2 = l_m2.createNewItem( "some concurrent item" );
    l_m2.save( "tmp_export.yaml" );


    // now the first model decides to dump it's mind
    l_m1.save( "tmp_export.yaml" );

    // client 2 (or a third client) starts again and loads the current db
    ThingsModel l_m3;
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
