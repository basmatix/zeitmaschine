/// -*- coding: utf-8 -*-
///
/// file: zmTypes.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZM_TYPES_H
#define ZM_TYPES_H

#include <string>
#include <map>
#include <list>

namespace zm {

class MindMatter;

typedef std::string                             uid_t;
typedef std::list< uid_t >                      uid_lst_t;
typedef std::pair< uid_t, int >                 neighbour_t;
typedef std::map< std::string, std::string >    string_value_map_type;
typedef std::pair< MindMatter *, neighbour_t >  item_neighbour_pair_t;
typedef std::map< MindMatter *, neighbour_t >   item_neighbour_map_t;

}

#endif
