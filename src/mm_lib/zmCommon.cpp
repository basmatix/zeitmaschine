/// -*- coding: utf-8 -*-
///
/// file: zmCommon.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/mm/zmCommon.h"

#include <boost/date_time.hpp>
#include <boost/regex.hpp>

std::time_t zm::common::seconds_from_epoch(const std::string& a_time, const std::string& a_format )
{
    boost::posix_time::ptime l_ptime;

    std::istringstream l_stringstream( a_time );
    boost::posix_time::time_input_facet l_facet(a_format,1);

    l_stringstream.imbue(std::locale(std::locale::classic(), &l_facet));

    l_stringstream >> l_ptime;
    if(l_ptime == boost::posix_time::ptime())
    {
        //TRACE_E(("could not parse time string"));
        return 0;
    }
    // std::cout << " ptime is " << l_ptime << std::endl;
    boost::posix_time::ptime timet_start(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration l_diff = l_ptime - timet_start;

    return l_diff.ticks() / boost::posix_time::time_duration::rep_type::ticks_per_second;
}

std::string zm::common::time_stamp_iso_ext()
{
    return boost::posix_time::to_iso_extended_string(
                boost::posix_time::microsec_clock::local_time());
}

std::string zm::common::time_stamp_iso()
{
    std::stringstream l_stream;

    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y%m%dT%H%M%S");
    l_stream.imbue(std::locale(l_stream.getloc(), facet));
    l_stream <<  boost::posix_time::second_clock::local_time();
    return l_stream.str();
}

std::time_t zm::common::pt_to_time_t(const boost::posix_time::ptime& pt)
{
    boost::posix_time::ptime timet_start(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration diff = pt - timet_start;
    std::time_t r = diff.ticks()/boost::posix_time::time_duration::rep_type::ticks_per_second;
    return r;
}

void zm::common::escapeRegex( std::string &regex )
{
    boost::replace_all(regex, "\\", "\\\\");
    boost::replace_all(regex, "^", "\\^");
    boost::replace_all(regex, ".", "\\.");
    boost::replace_all(regex, "$", "\\$");
    boost::replace_all(regex, "|", "\\|");
    boost::replace_all(regex, "(", "\\(");
    boost::replace_all(regex, ")", "\\)");
    boost::replace_all(regex, "[", "\\[");
    boost::replace_all(regex, "]", "\\]");
    boost::replace_all(regex, "*", "\\*");
    boost::replace_all(regex, "+", "\\+");
    boost::replace_all(regex, "?", "\\?");
    boost::replace_all(regex, "/", "\\/");
}

bool zm::common::matchesWildcards( const std::string &text, std::string wildcardPattern, bool caseSensitive )
{
    // Escape all regex special chars
    escapeRegex(wildcardPattern);

    // Convert chars '*?' back to their regex equivalents
    boost::replace_all(wildcardPattern, "\\?", ".");
    boost::replace_all(wildcardPattern, "\\*", ".*");

    boost::regex pattern(wildcardPattern, caseSensitive ? boost::regex::normal : boost::regex::icase);

    return regex_match(text, pattern);
}
