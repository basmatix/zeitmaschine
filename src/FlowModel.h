#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include <map>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <boost/foreach.hpp>

class Thing
{
public:

    Thing( const std::string &caption)
        : m_caption   ( caption )
    {
    }

    std::string m_caption;
};

class Model
{
public:
    typedef std::map<  std::string, Thing * > FlowModelMapType;
    //    QMutex      m_mutex;
    //    QMutexLocker monitor( &m_mutex );


    FlowModelMapType m_things;

private:

    // returns 16x8 bit
    static inline std::string generateUid()
    {
        std::string l_return;
        l_return.reserve(16);
        l_return.resize(16);
        for( int i = 0; i < 16; ++i )
        {
            const char *l_characters = "0123456789abcdef";
            l_return[i] = l_characters[ rand() % 16 ];
        }
        return l_return;
    }

    void clear()
    {
        BOOST_FOREACH(const FlowModelMapType::value_type& i, m_things)
        {
            delete i.second;
        }
        m_things.clear();
    }

public:

    void setModelFolder()
    {

    }

    void load( const std::string &filename )
    {
        clear();

        YAML::Node l_import = YAML::LoadFile(filename);

        BOOST_FOREACH( YAML::Node n, l_import )
        {
            assert( n["uid"] );
            assert( n["caption"] );

            std::string l_uid =     n["uid"    ].as< std::string >();
            std::string l_caption = n["caption"].as< std::string >();

            Thing *l_new_thing = new Thing( l_caption );
            m_things[ l_uid ] = l_new_thing;
        }
    }

    void save( const std::string &filename )
    {
        YAML::Node l_export_root;

        BOOST_FOREACH(const FlowModelMapType::value_type& i, m_things)
        {
            YAML::Node l_export_item;
            l_export_item["uid"    ] = i.first;
            l_export_item["caption"] = i.second->m_caption;
            l_export_root.push_back( l_export_item );
        }

        std::ofstream l_fout( filename.c_str() );
        l_fout << l_export_root;
        l_fout << std::endl;
    }

    void createNewItem( const std::string &caption )
    {
        std::string l_new_key = generateUid();
        Thing *l_new_thing = new Thing( caption );
        m_things[ l_new_key ] = l_new_thing;
    }
};

#endif
