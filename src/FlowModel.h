#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include <map>
#include <fstream>
#include <string>
#include <json/writer.h>
//#include <json/reader.h>

class Thing
{
public:

    Thing()
        : m_id            ( "" )
    {
    }

    std::string m_id;
};

class Model
{
public:
    void setModelFolder()
    {

    }

    void save( const std::string &filename )
    {
        Json::FastWriter l_writer;
        Json::Value l_root;
        l_root.append("test");

        std::string l_json_string = l_writer.write(l_root);
        std::ofstream l_out_stream( filename.c_str() );
        l_out_stream << l_json_string;
    }

    void createNewItem( const std::string &caption )
    {

    }

//    QMutex      m_mutex;
//    QMutexLocker monitor( &m_mutex );

    std::map< std::string, Thing > m_things;
};

#endif
