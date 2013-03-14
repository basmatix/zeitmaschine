#ifndef ZMGTDMODEL_H
#define ZMGTDMODEL_H

#include <zmModel.h>

class zmGtdModel
{
    ThingsModel m_things_model;

public:

    const ThingsModel::ThingsModelMapType & things() const
    {
        return m_things_model.things();
    }

    void load( const std::string &filename )
    {
        return m_things_model.load( filename );
    }

    void save( const std::string &filename )
    {
        return m_things_model.save( filename );
    }

    std::string createNewItem( const std::string &caption )
    {
        return m_things_model.createNewItem( caption );
    }

    void eraseItem( const std::string &uid )
    {
        return m_things_model.eraseItem( uid );
    }

    void addAttribute( const std::string &uid, const std::string &attribute )
    {
        return m_things_model.addAttribute( uid, attribute );
    }

    bool removeAttribute( const std::string &uid, const std::string &attribute )
    {
        return m_things_model.removeAttribute( uid, attribute );
    }

    void setValue( const std::string &uid, const std::string &name, const std::string &value )
    {
        return m_things_model.setValue( uid, name, value );
    }

    void setCaption( const std::string &uid, const std::string &caption )
    {
        return m_things_model.setCaption( uid, caption );
    }

    const std::string & getCaption( const std::string &uid ) const
    {
        return m_things_model.getCaption( uid );
    }

    bool hasAttribute( const std::string uid, const std::string attribute ) const
    {
        return m_things_model.hasAttribute( uid, attribute );
    }
};

#endif
