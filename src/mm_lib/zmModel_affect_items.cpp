/// -*- coding: utf-8 -*-
///
/// file: zmModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <mm/zmModel.h>

#include "zmThing.h"

#include <mm/zmTrace.h>

using namespace zm;

zm::uid_lst_t zm::MindMatterModel::getFolderChildren(
        const zm::uid_t &folder_item ) const
{
    uid_lst_t l_result;
    return l_result;
}

std::time_t zm::MindMatterModel::getCreationTime(
        const std::string &uid ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    std::string l_timeStr = l_item_it->second->getValue( "global_time_created" );

    // unfortunately in boost 1.50 iso_format_extended_specifier only specifies to "%Y-%m-%dT"
    //return seconds_from_epoch( l_timeStr, boost::posix_time::time_facet::iso_format_extended_specifier );
    return zm::common::seconds_from_epoch( l_timeStr, "%Y-%m-%dT%H:%M:%S" );
}

bool zm::MindMatterModel::hasItem(
        const std::string &uid ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    return l_item_it != m_things.left.end();
}

std::string zm::MindMatterModel::getValue(
        const std::string &uid,
        const std::string &name ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    return l_item_it->second->getValue( name );
}

std::string zm::MindMatterModel::getCaption(
        const std::string &uid ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_s( l_item_it != m_things.left.end() );

    if(l_item_it == m_things.left.end())
    {
        return "";
    }
    return l_item_it->second->m_caption;
}

std::string zm::MindMatterModel::_getCaption(
        ModelData::left_const_iterator &a_item ) const
{
    if(a_item == m_things.left.end())
    {
        return "";
    }
    return a_item->second->m_caption;
}

bool zm::MindMatterModel::hasTag(
        const std::string &uid,
        const std::string &tag_name ) const
{
    /// check and assert item <uid> exists
    ModelData::left_const_iterator l_item_it(
                m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    ModelData::left_const_iterator l_tag_it(
                m_things.left.find( tag_name ) );

    if( l_tag_it == m_things.left.end() )
    {
        return false;
    }

    return _isConnected( l_item_it, l_tag_it );
}

bool zm::MindMatterModel::isTag(
        const zm::uid_t &uid ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    return l_item_it->first == l_item_it->second->m_caption;
}

bool zm::MindMatterModel::hasValue(
        const std::string &uid,
        const std::string &name ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    return l_item_it->second->hasValue( name );
}

bool zm::MindMatterModel::itemContentMatchesString(
        const std::string &uid,
        const std::string &searchString ) const
{
    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    return l_item_it->second->contentMatchesString( searchString );
}

bool zm::MindMatterModel::isConnected(
        const std::string &node1_uid,
        const std::string &node2_uid ) const
{
    ModelData::left_const_iterator l_item1_it(
                m_things.left.find( node1_uid ) );

    ModelData::left_const_iterator l_item2_it(
                m_things.left.find( node2_uid ) );

    trace_assert_h( l_item1_it != m_things.left.end() );
    trace_assert_h( l_item2_it != m_things.left.end() );

    return _isConnected( l_item1_it, l_item2_it );
}

bool zm::MindMatterModel::_isConnected(
        ModelData::left_const_iterator a_item1_it,
        ModelData::left_const_iterator a_item2_it ) const
{
    item_neighbour_map_t &n1( a_item1_it->second->m_neighbours );
    item_neighbour_map_t &n2( a_item2_it->second->m_neighbours );

    MindMatter *l_item1_ptr( a_item1_it->second );
    MindMatter *l_item2_ptr( a_item2_it->second );

    // this check is just for performance reasons (we test the
    // smaller set (in case assert expands to void))
    if( n1.size() < n2.size() )
    {
        if( n1.find( l_item2_ptr ) != n1.end() )
        {
            /// assert item1 is in connection list of item2
            trace_assert_h( n2.find( l_item1_ptr ) != n2.end() );
            return true;
        }
        else
        {
            /// assert item1 is NOT in connection list of item2
            trace_assert_h( n2.find( l_item1_ptr ) == n2.end() );
            return false;
        }
    }
    else
    {
        if( n2.find( l_item1_ptr ) != n2.end() )
        {
            /// assert item2 is in connection list of item1
            trace_assert_h( n1.find( l_item2_ptr ) != n1.end() );
            return true;
        }
        else
        {
            /// assert item2 is NOT in connection list of item1
            trace_assert_h( n1.find( l_item2_ptr ) == n1.end() );
            return false;
        }
    }
}

zm::uid_lst_t zm::MindMatterModel::getNeighbours(
        const std::string &node_uid ) const
{
    zm::uid_lst_t l_result;

    ModelData::left_const_iterator l_item_it( m_things.left.find( node_uid ) );
    trace_assert_h( l_item_it != m_things.left.end() );

    for( const item_neighbour_pair_t &m: l_item_it->second->m_neighbours)
    {
        { /// just for checking
            ModelData::right_const_iterator i = m_things.right.find(m.first);
            trace_assert_h( i != m_things.right.end() );
        }
        l_result.push_back( m.second.first );
    }

    return l_result;
}

zm::uid_lst_t zm::MindMatterModel::getTags(
        const zm::uid_t   &uid ) const
{
    zm::uid_lst_t l_result;

    ModelData::left_const_iterator l_item_it( m_things.left.find( uid ) );
    trace_assert_h( l_item_it != m_things.left.end() );

    for( const item_neighbour_pair_t &m: l_item_it->second->m_neighbours )
    {
        if( isTag( m.second.first ) )
        {
            l_result.push_back( m.second.first );
        }
    }

    return l_result;
}

zm::uid_t zm::MindMatterModel::findOrCreateTagItem(
        const std::string &tag_name )
{
    ModelData::left_iterator l_item_it( m_things.left.find( tag_name ) );

    if( l_item_it != m_things.left.end() )
    {
        return tag_name;
    }

    // todo: TagItems should be held in a list to prevent generic Items
    //       from getting the same name
    // todo: what about tag items from other domains? Should there be duplicated
    //       tags?

    return createNewItem( tag_name, tag_name );
}

///
/// write relevant interface
///
zm::uid_t zm::MindMatterModel::createNewItem(
        const std::string   &caption,
        const zm::uid_t     &uid )
{
    return _createNewItem(m_things, caption, uid);
}

zm::uid_t zm::MindMatterModel::_createNewItem(
              ModelData     &model,
        const std::string   &caption,
        const zm::uid_t     &uid )
{
    std::string l_time = zm::common::time_stamp_iso_ext();
    zm::uid_t   l_new_key( uid );

    if( uid == "" )
    {
        l_new_key = generateUid();
    }

    trace_assert_h( model.left.find( l_new_key ) == model.left.end() );

    _createNewItem( model, l_new_key, caption, l_time );

    return l_new_key;
}

void zm::MindMatterModel::_createNewItem(
              ModelData     &a_model,
        const zm::uid_t     &a_uid,
        const std::string   &a_caption,
        const std::string   &a_time )
{
    MindMatter *l_new_thing = new MindMatter( a_caption );
    if( a_uid != a_caption )
    {
        l_new_thing->addValue( "global_time_created", a_time );
    }
    a_model.insert( ModelData::value_type(a_uid, l_new_thing) );
}

void zm::MindMatterModel::eraseItem( const zm::uid_t &uid )
{
    ModelData::left_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    _eraseItem(m_things, l_item_it);
}

void zm::MindMatterModel::_eraseItem(
        ModelData           &a_model,
        ModelData::left_iterator &item )
{
    for( const item_neighbour_pair_t &m: item->second->m_neighbours )
    {
        //<MindMatter*, <uid_t, int> >
        _disconnect(item->second, m.first);
    }
    //todo: delete item
    a_model.left.erase( item );
}

void zm::MindMatterModel::addTag(
        const zm::uid_t     &uid,
        const std::string   &tag_name )
{
    ModelData::left_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    _addTag( m_things, l_item_it, tag_name );
}

void zm::MindMatterModel::_addTag(
        ModelData                  &model,
        ModelData::left_iterator   &item,
        const std::string          &tag_name )
{
    ModelData::left_iterator l_tag_it( model.left.find( tag_name ) );

    if( l_tag_it == model.left.end() )
    {
        std::string n(_createNewItem( model, tag_name, tag_name ));
        ModelData::left_iterator i( model.left.find( n ) );
        _connectDuplex( item, i, Directed );
    }
    else
    {
        _connectDuplex( item, l_tag_it, Directed );
    }
}

void zm::MindMatterModel::becomeFolder(
        const uid_t &item )
{
    // todo: should not be possible if already connected bidirectionally with folder
    addTag(item, "tsk_folder");
}

void zm::MindMatterModel::putIntoFolder(
        const uid_t &item,
        const uid_t &folder)
{
    // todo: what should happen if folders get connected bidirectionally
    connectDirected(folder, item);
}

bool zm::MindMatterModel::removeTag(
        const std::string &uid,
        const std::string &tag_name )
{
    ModelData::left_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    bool l_return = _removeTag( l_item_it, tag_name );

    return l_return;
}

bool zm::MindMatterModel::_removeTag(
              ModelData::left_iterator  &a_item_it,
        const std::string               &tag_name )
{
    ModelData::left_iterator l_tag_it( m_things.left.find( tag_name ) );

    if( l_tag_it == m_things.left.end() )
    {
        return false;
    }

    if( _isConnected( a_item_it, l_tag_it ) )
    {
        a_item_it->second->m_neighbours.erase( l_tag_it->second );
        l_tag_it->second->m_neighbours.erase( a_item_it->second );
        return true;
    }

    return false;
}

void zm::MindMatterModel::disconnect(
        const std::string &node1_uid,
        const std::string &node2_uid )
{
    ModelData::left_iterator l_item1_it( m_things.left.find( node1_uid ) );
    ModelData::left_iterator l_item2_it( m_things.left.find( node2_uid ) );

    trace_assert_h( l_item1_it != m_things.left.end() );
    trace_assert_h( l_item2_it != m_things.left.end() );

    trace_assert_s( _isConnected( l_item1_it, l_item2_it ) );

    _disconnect(l_item2_it, l_item1_it);
}

void zm::MindMatterModel::connectDirected(
        const std::string &a_node1_uid,
        const std::string &a_node2_uid )
{
    ModelData::left_iterator l_item1_it(
                m_things.left.find( a_node1_uid ) );

    ModelData::left_iterator l_item2_it(
                m_things.left.find( a_node2_uid ) );

    trace_assert_h( l_item1_it != m_things.left.end() );
    trace_assert_h( l_item2_it != m_things.left.end() );

    _connectDuplex( l_item1_it, l_item2_it, Directed );
}

void zm::MindMatterModel::_connectDuplex(
        ModelData::left_iterator &item1,
        ModelData::left_iterator &item2,
        zm::MindMatterModel::ConnectionType type)
{
    // for now assert Directed connections - this is not meant to persist
    trace_assert_h(type == Directed);

    // assert changes: eighter there is no connection yet or the types differ
    trace_assert_s(
              item1->second->m_neighbours.find(item2->second) == item1->second->m_neighbours.end()
           || item1->second->m_neighbours[item2->second].second != 2);

    trace_assert_s(
              item2->second->m_neighbours.find(item1->second) == item2->second->m_neighbours.end()
           || item2->second->m_neighbours[item1->second].second != 1);

    item1->second->m_neighbours[item2->second] = zm::neighbour_t(item2->first, 2);
    item2->second->m_neighbours[item1->second] = zm::neighbour_t(item1->first, 1);
}

void zm::MindMatterModel::_connectSingle(
        ModelData::left_iterator &item1,
        ModelData::left_iterator &item2,
        int type)
{
    item1->second->m_neighbours[item2->second] = zm::neighbour_t(item2->first, type);
}

void zm::MindMatterModel::_disconnect(
        ModelData::left_iterator &a_item1,
        ModelData::left_iterator &a_item2 )
{
    a_item1->second->m_neighbours.erase( a_item2->second );
    a_item2->second->m_neighbours.erase( a_item1->second );
}

void zm::MindMatterModel::_disconnect(
        MindMatter *a_item1,
        MindMatter *a_item2 )
{
    a_item1->m_neighbours.erase( a_item2 );
    a_item2->m_neighbours.erase( a_item1 );
}

void zm::MindMatterModel::setValue(
        const std::string &uid,
        const std::string &name,
        const std::string &value )
{
    ModelData::left_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    if( !_setValue(l_item_it, name, value ) ) return;
}

bool zm::MindMatterModel::_setValue(
              ModelData::left_iterator  &item,
        const std::string               &name,
        const std::string               &value )
{
    if( item->second->hasValue( name )
     && item->second->getValue( name ) == value ) return false;
    item->second->addValue( name, value );
    return true;
}

void zm::MindMatterModel::setCaption(
        const std::string &uid,
        const std::string &caption )
{
    ModelData::left_iterator l_item_it( m_things.left.find( uid ) );

    trace_assert_h( l_item_it != m_things.left.end() );

    if( !_setCaption( l_item_it, caption ) ) return;
}

bool zm::MindMatterModel::_setCaption(
              ModelData::left_iterator &item,
        const std::string &caption )
{
    if( item->second->m_caption == caption ) return false;
    item->second->m_caption = caption;
    return true;
}
