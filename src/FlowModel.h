#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include <map>
#include <string>

// returns 16x8 bit
std::string generateUid()
{
    std::string l_return;
    l_return.reserve(16);
    l_return.resize(16);
    for( int i = 0; i < 16; ++i )
    {
        const char *l_characters="0123456789abcdef";
        l_return[i] = l_characters[4];
    }
    return l_return;
}

class Thing
{
public:

    Thing( const std::string & caption )
        : uid     ( generateUid() )
        , caption( caption )
    {
    }

    std::string uid;
    std::string caption;
};

class Model
{
public:
    void setModelFolder()
    {

    }

    void createNewItem( const std::string &caption )
    {
        Thing *l_newThing = new Thing( caption );
        m_things[ l_newThing->caption ] = l_newThing;
    }

private:

//    QMutex      m_mutex;
//    QMutexLocker monitor( &m_mutex );

    std::map< std::string, Thing* > m_things;
};

#endif
