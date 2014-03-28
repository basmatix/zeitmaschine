/// -*- coding: utf-8 -*-
///
/// file: zmModelData.h
///
/// Copyright (C) 2014 Frans Fuerst
///

#ifndef ZMMODELDATA_H
#define ZMMODELDATA_H


#include <boost/bimap.hpp>
#include <string>

namespace zm
{
    class MindMatter;

    class ModelData
    {
        typedef boost::bimaps::bimap< std::string, MindMatter * > uid_mm_bimap_t;
        uid_mm_bimap_t m_data;

    public:

        std::set< std::string > m_read_journals;

        ModelData()
            : m_data    ()
            , right     (m_data.right)
            , left      (m_data.left)
            , m_read_journals()
        { }

        typedef uid_mm_bimap_t::left_const_iterator left_const_iterator;
        typedef uid_mm_bimap_t::left_iterator left_iterator;
        typedef uid_mm_bimap_t::right_const_iterator right_const_iterator;
        typedef uid_mm_bimap_t::right_iterator right_iterator;
        typedef uid_mm_bimap_t::const_iterator const_iterator;
        typedef uid_mm_bimap_t::iterator iterator;

        typedef uid_mm_bimap_t::value_type value_type;

        virtual ~ModelData(){}

        size_t size() const {return m_data.size();}
        bool empty() const {return m_data.empty();}
        void clear() {m_data.clear();}
        void insert(value_type a_pair) {m_data.insert(a_pair);}
        uid_mm_bimap_t::iterator begin() {return m_data.begin();}
        uid_mm_bimap_t::iterator end() {return m_data.end();}
        uid_mm_bimap_t::const_iterator begin() const {return m_data.begin();}
        uid_mm_bimap_t::const_iterator end() const{return m_data.end();}

        uid_mm_bimap_t::left_map  &left;
        uid_mm_bimap_t::right_map &right;

    public:
        void debug_dump() const;

    };
}

#endif
