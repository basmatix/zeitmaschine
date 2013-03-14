#ifndef ZMGTDMODEL_H
#define ZMGTDMODEL_H

#include <zmModel.h>

class zmGtdModel
{
    ThingsModel m_things_model;

///
/// GTD specific interface
///
public:

    void registerItemAsTask( const std::string &task_item, const std::string &project_item )
    {
        assert( isInboxItem( task_item ) );
        assert( isProjectItem( project_item ) );

        m_things_model.removeAttribute( task_item, "gtd_item_unhandled" );
        m_things_model.addAttribute( task_item, "gtd_task" );
        m_things_model.setValue( task_item, "gtd_parent_project", project_item );
    }

    std::list< std::string > getInboxItems()
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isInboxItem( i.first ))
            l_return.push_back( i.first );
        }

        return l_return;
    }

    std::list< std::string > getTaskItems()
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isTaskItem( i.first ))
            l_return.push_back( i.first );
        }

        return l_return;
    }

    std::list< std::string > getProjectItems()
    {
        std::list< std::string > l_return;

        BOOST_FOREACH(const ThingsModel::ThingsModelMapType::value_type& i, m_things_model.things() )
        {
            if( isProjectItem( i.first ))
            l_return.push_back( i.first );
        }

        return l_return;
    }

    void setDone( const std::string &task_item )
    {
        m_things_model.removeAttribute(
                    task_item, "gtd_item_unhandled" );

        m_things_model.addAttribute(
                    task_item, "gtd_item_done" );

        m_things_model.setValue(
                    task_item, "gtd_time_done", ThingsModel::time_stamp() );
    }

    bool isTaskItem( const std::string &item )
    {
        return m_things_model.hasAttribute( item, "gtd_task" );
    }

    bool isInboxItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_item_unhandled" );
    }

    bool isProjectItem( const std::string &item ) const
    {
        return m_things_model.hasAttribute( item, "gtd_project" );
    }

    bool isDone( const std::string &task_item ) const
    {
        return m_things_model.hasAttribute( task_item, "gtd_item_done" );
    }

    std::string getParentProject( const std::string &task_item )
    {
        assert( isTaskItem( task_item ) );
        assert( hasValue( task_item, "gtd_parent_project" ) );

        return m_things_model.getValue( task_item, "gtd_parent_project" );
    }

    std::string createProject( const std::string &project_name )
    {
        std::string l_item_uid = m_things_model.createNewItem( project_name );

        m_things_model.addAttribute( l_item_uid, "gtd_project" );

        return l_item_uid;
    }

///
/// low level interface - to be vanished
///
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

    bool hasValue( const std::string uid, const std::string attribute ) const
    {
        return m_things_model.hasAttribute( uid, attribute );
    }

    std::string getValue( const std::string uid, const std::string name ) const
    {
        return m_things_model.getValue( uid, name );
    }
};

#endif
