#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include <map>
#include <set>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

class Thing
{
public:

    Thing( const std::string &caption)
        : m_caption   ( caption )
    {
    }

    void addAttribute( const std::string &attribute )
    {
        m_attributes.insert( attribute );
    }

    bool hasAttribute( const std::string &attribute ) const
    {
        return m_attributes.find( attribute ) != m_attributes.end();
    }

    std::string m_caption;
    std::set< std::string > m_attributes;
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

        if( ! boost::filesystem::exists( filename ) )
        {
            return;
        }
        YAML::Node l_import = YAML::LoadFile(filename);

        BOOST_FOREACH( YAML::Node n, l_import )
        {
            assert( n["uid"] );
            assert( n["caption"] );

            std::string l_uid =     n["uid"    ].as< std::string >();
            std::string l_caption = n["caption"].as< std::string >();

            Thing *l_new_thing = new Thing( l_caption );
            if( n["attributes"] )
            {
                std::vector< std::string > l_attributes =
                        n["attributes"].as< std::vector< std::string > >();
                for( std::vector< std::string >::const_iterator
                     a  = l_attributes.begin();
                     a != l_attributes.end(); ++ a )
                {
                    l_new_thing->m_attributes.insert( *a );
                }
            }
            m_things[ l_uid ] = l_new_thing;
        }
    }

    void save( const std::string &filename )
    {
        std::string l_dir = filename.substr( 0, filename.find_last_of("/") );

        try
        {
            boost::filesystem::create_directory( l_dir );
        }
        catch( ... )
        {
            //error
        }

        std::ofstream l_fout( filename.c_str() );

        if( ! m_things.empty() )
        {
            YAML::Node l_export_root;

            BOOST_FOREACH(const FlowModelMapType::value_type& i, m_things)
            {
                YAML::Node l_export_item;
                l_export_item["uid"    ] = i.first;
                l_export_item["caption"] = i.second->m_caption;

                if( ! i.second->m_attributes.empty() )
                {
                    std::vector< std::string > v;
                    BOOST_FOREACH( const std::string &a, i.second->m_attributes)
                    {
                        v.push_back( a );
                    }

                    l_export_item["attributes"] = v;
                }

                l_export_root.push_back( l_export_item );
            }

            l_fout << l_export_root;
        }
        l_fout << std::endl;
    }

    std::string createNewItem( const std::string &caption )
    {
        std::string l_new_key = generateUid();
        Thing *l_new_thing = new Thing( caption );
        m_things[ l_new_key ] = l_new_thing;
        return l_new_key;
    }

    void addAttribute( const std::string &uid, const std::string &attribute )
    {
        FlowModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        l_item_it->second->addAttribute( attribute );
    }

    std::string getCaption( const std::string &uid )
    {
        FlowModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->m_caption;
    }

    bool hasAttribute( const std::string uid, const std::string attribute )
    {
        FlowModelMapType::iterator l_item_it( m_things.find( uid ) );

        assert( l_item_it != m_things.end() );

        return l_item_it->second->hasAttribute( attribute );
    }
};

#endif
