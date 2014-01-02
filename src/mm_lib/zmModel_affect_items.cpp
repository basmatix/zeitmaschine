/// -*- coding: utf-8 -*-
///
/// file: zmModel.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <mm/zmModel.h>

#include "zmThing.h"

#include <mm/zmTrace.h>
#include <boost/foreach.hpp>

using namespace zm;

std::time_t zm::MindMatterModel::getCreationTime( const std::string &uid ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    std::string l_timeStr = l_item_it->second->getValue( "global_time_created" );

    // unfortunately in boost 1.50 iso_format_extended_specifier only specifies to "%Y-%m-%dT"
    //return seconds_from_epoch( l_timeStr, boost::posix_time::time_facet::iso_format_extended_specifier );
    return zm::common::seconds_from_epoch( l_timeStr, "%Y-%m-%dT%H:%M:%S" );
}

bool zm::MindMatterModel::hasItem( const std::string &uid ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    return l_item_it != m_things.left.end();
}

std::string zm::MindMatterModel::getValue( const std::string &uid, const std::string &name ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    return l_item_it->second->getValue( name );
}

const std::string & zm::MindMatterModel::getCaption( const std::string &uid ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    return l_item_it->second->m_caption;
}

bool zm::MindMatterModel::hasTag(
        const std::string &uid,
        const std::string &tag_name ) const
{
    /// check and assert item <uid> exists
    uid_mm_bimap_t::left_const_iterator l_item_it(
                m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    uid_mm_bimap_t::left_const_iterator l_tag_it(
                m_things.left.find( tag_name ) );

    if( l_tag_it == m_things.left.end() )
    {
        return false;
    }

    return _isConnected( l_item_it, l_tag_it );
}

bool zm::MindMatterModel::hasValue( const std::string &uid, const std::string &name ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    return l_item_it->second->hasValue( name );
}

bool zm::MindMatterModel::itemContentMatchesString(
        const std::string &uid,
        const std::string &searchString ) const
{
    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    return l_item_it->second->contentMatchesString( searchString );
}

bool zm::MindMatterModel::isConnected(
        const std::string &node1_uid,
        const std::string &node2_uid ) const
{
    uid_mm_bimap_t::left_const_iterator l_item1_it(
                m_things.left.find( node1_uid ) );

    uid_mm_bimap_t::left_const_iterator l_item2_it(
                m_things.left.find( node2_uid ) );

    assert( l_item1_it != m_things.left.end() );
    assert( l_item2_it != m_things.left.end() );

    return _isConnected( l_item1_it, l_item2_it );
}

bool zm::MindMatterModel::_isConnected(
        uid_mm_bimap_t::left_const_iterator a_item1_it,
        uid_mm_bimap_t::left_const_iterator a_item2_it ) const
{
    MindMatter::item_uid_map_t &n1( a_item1_it->second->m_neighbours );
    MindMatter::item_uid_map_t &n2( a_item2_it->second->m_neighbours );

    // this check is just for performance reasons (we test the
    // smaller set (in case assert expands to void))
    if( n1.size() < n2.size() )
    {
        if( n1.find( a_item2_it->second ) != n1.end() )
        {
            assert( n2.find( a_item1_it->second ) != n2.end() );
            return true;
        }
        else
        {
            assert( n2.find( a_item1_it->second ) == n2.end() );
            return false;
        }
    }
    else
    {
        if( n2.find( a_item1_it->second ) != n2.end() )
        {
            assert( n1.find( a_item2_it->second ) != n1.end() );
            return true;
        }
        else
        {
            assert( n1.find( a_item2_it->second ) == n1.end() );
            return false;
        }
    }
}

std::list< std::string > zm::MindMatterModel::getNeighbours(
        const std::string &node_uid ) const
{
    std::list< std::string > l_result;

    uid_mm_bimap_t::left_const_iterator l_item_it( m_things.left.find( node_uid ) );
    assert( l_item_it != m_things.left.end() );

    BOOST_FOREACH( const MindMatter::item_uid_pair_t &m,
                   l_item_it->second->m_neighbours)
    {
        uid_mm_bimap_t::right_const_iterator i = m_things.right.find(m.first);
        assert( i != m_things.right.end() );
        l_result.push_back( i->second  );
    }

    return l_result;
}

std::string zm::MindMatterModel::findOrCreateTagItem( const std::string &tag_name )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( tag_name ) );

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
std::string zm::MindMatterModel::createNewItem(
        const std::string &caption,
        const std::string &uid )
{
    return _createNewItem(m_things, caption, uid);
}

std::string zm::MindMatterModel::_createNewItem(
        uid_mm_bimap_t    &model,
        const std::string &caption,
        const std::string &uid )
{
    std::string l_time = zm::common::time_stamp_iso_ext();
    std::string l_new_key( uid );

    if( uid == "" )
    {
        l_new_key = generateUid();
    }

    assert( model.left.find( l_new_key ) == model.left.end() );

    _createNewItem( model, l_new_key, caption, l_time );

    return l_new_key;
}

void zm::MindMatterModel::_createNewItem(
        uid_mm_bimap_t    &model,
        const std::string &a_uid,
        const std::string &a_caption,
        const std::string &a_time )
{
    MindMatter *l_new_thing = new MindMatter( a_caption );
    l_new_thing->addValue( "global_time_created", a_time );
    model.insert( uid_mm_bimap_t::value_type(a_uid,l_new_thing) );
}

void zm::MindMatterModel::eraseItem( const std::string &uid )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    _eraseItem( l_item_it );
}

void zm::MindMatterModel::_eraseItem( uid_mm_bimap_t::left_iterator &item )
{
    //todo: remove references
    //todo: delete item
    //m_things.erase( item );
    m_things.left.erase( item );
}

void zm::MindMatterModel::addTag(
        const std::string &uid,
        const std::string &tag_name )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    _addTag( m_things, l_item_it, tag_name );
}

void zm::MindMatterModel::_addTag(
        uid_mm_bimap_t                  &model,
        uid_mm_bimap_t::left_iterator   &item,
        const std::string               &tag_name )
{
    uid_mm_bimap_t::left_iterator l_tag_it( model.left.find( tag_name ) );

    if( l_tag_it == model.left.end() )
    {
        std::string n(_createNewItem( model, tag_name, tag_name ));
        uid_mm_bimap_t::left_iterator i( model.left.find( n ) );
        _connect( item, i );
    }
    else
    {
        _connect( item, l_tag_it );
    }
}

bool zm::MindMatterModel::removeTag(
        const std::string &uid,
        const std::string &tag_name )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    bool l_return = _removeTag( l_item_it, tag_name );

    return l_return;
}

bool zm::MindMatterModel::_removeTag(
        uid_mm_bimap_t::left_iterator   &a_item_it,
        const std::string               &tag_name )
{
    uid_mm_bimap_t::left_iterator l_tag_it( m_things.left.find( tag_name ) );

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

void zm::MindMatterModel::disconnect( const std::string &node1_uid, const std::string &node2_uid )
{
    uid_mm_bimap_t::left_iterator l_item1_it( m_things.left.find( node1_uid ) );
    uid_mm_bimap_t::left_iterator l_item2_it( m_things.left.find( node2_uid ) );

    assert( l_item1_it != m_things.left.end() );
    assert( l_item2_it != m_things.left.end() );

    assert( _isConnected( l_item1_it, l_item2_it ) );

    _disconnect(l_item2_it, l_item1_it);
}

void zm::MindMatterModel::connect(
        const std::string &a_node1_uid,
        const std::string &a_node2_uid )
{
    uid_mm_bimap_t::left_iterator l_item1_it(
                m_things.left.find( a_node1_uid ) );

    uid_mm_bimap_t::left_iterator l_item2_it(
                m_things.left.find( a_node2_uid ) );

    assert( l_item1_it != m_things.left.end() );
    assert( l_item2_it != m_things.left.end() );

    _connect( l_item1_it, l_item2_it );
}

void zm::MindMatterModel::_connect(
        uid_mm_bimap_t::left_iterator &item1,
        uid_mm_bimap_t::left_iterator &item2 )
{
    item1->second->m_neighbours[item2->second] = item2->first;
    item2->second->m_neighbours[item1->second] = item1->first;
}

void zm::MindMatterModel::_disconnect(
        uid_mm_bimap_t::left_iterator &a_item1,
        uid_mm_bimap_t::left_iterator &a_item2 )
{
    a_item1->second->m_neighbours.erase( a_item2->second );
    a_item2->second->m_neighbours.erase( a_item1->second );
}

void zm::MindMatterModel::setValue(
        const std::string &uid,
        const std::string &name,
        const std::string &value )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    if( !_setValue(l_item_it, name, value ) ) return;
}

bool zm::MindMatterModel::_setValue(
        uid_mm_bimap_t::left_iterator   &item,
        const std::string                       &name,
        const std::string                       &value )
{
    if( item->second->hasValue( name )
     && item->second->getValue( name ) == value ) return false;
    item->second->addValue( name, value );
    return true;
}

void zm::MindMatterModel::setCaption( const std::string &uid, const std::string &caption )
{
    uid_mm_bimap_t::left_iterator l_item_it( m_things.left.find( uid ) );

    assert( l_item_it != m_things.left.end() );

    if( !_setCaption( l_item_it, caption ) ) return;
}

bool zm::MindMatterModel::_setCaption( uid_mm_bimap_t::left_iterator &item, const std::string &caption )
{
    if( item->second->m_caption == caption ) return false;
    item->second->m_caption = caption;
    return true;
}
