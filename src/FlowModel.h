#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include <map>
#include <string>

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

    void createNewItem( const std::string &caption )
    {

    }

//    QMutex      m_mutex;
//    QMutexLocker monitor( &m_mutex );

    std::map< std::string, Thing > m_things;
};

#endif
