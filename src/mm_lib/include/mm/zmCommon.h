/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef ZMCOMMON_H
#define ZMCOMMON_H

#include <string>
#include <ctime>

namespace boost{ namespace posix_time{
    class ptime;
}}

namespace zm
{
namespace common
{

std::time_t seconds_from_epoch(const std::string& a_time, const std::string& a_format );

std::string time_stamp_iso_ext();

std::string time_stamp_iso();

std::time_t pt_to_time_t(const boost::posix_time::ptime& pt);

void escapeRegex( std::string &regex );

bool matchesWildcards( const std::string &text, std::string wildcardPattern, bool caseSensitive = true);

}
}
#endif
