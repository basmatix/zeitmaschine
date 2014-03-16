/// -*- coding: utf-8 -*-
///
/// file: zmCommon.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMCOMMON_H
#define ZMCOMMON_H

#include <string>
#include <vector>
#include <set>
#include <ctime>

namespace boost{ namespace posix_time{
    class ptime;
}}

namespace zm
{
namespace common
{

bool create_base_directory( const std::string& filename );

std::time_t seconds_from_epoch(const std::string& a_time, const std::string& a_format );

std::string time_stamp_iso_ext();

std::string time_stamp_iso();

std::time_t pt_to_time_t(const boost::posix_time::ptime& pt);

std::string to_string(int value);

void escapeRegex( std::string &regex );

bool matchesWildcards(
        const std::string &text,
        const std::string &wildcardPattern,
        bool caseSensitive = true);

std::vector<std::string> split(
        const std::string &input,
        const char *separators);

std::set< std::string > get_files_in_dir(
        const std::string &directory,
        const std::string &wildcardPattern);

void copy_file(
        const std::string &file_src,
        const std::string &file_dest);

}
}
#endif
