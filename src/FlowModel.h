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

    QMap< QString, Thing >   m_texts;
};

#endif
