/// -*- coding: utf-8 -*-
///
/// file: zmCommon.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include "include/mm/zmCommon.h"

#include <boost/date_time.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

bool zm::common::create_base_directory( const std::string& filename )
{
    if( filename.find_last_of("/") != std::string::npos )
    {
        std::string l_dir = filename.substr( 0, filename.find_last_of("/") );

        try
        {
            boost::filesystem::create_directory( l_dir );
        }
        catch( ... )
        {
            return false;
        }
    }
    return true;
}

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

bool zm::common::matchesWildcards(
        const std::string &text,
        const std::string &wildcardPattern,
        bool caseSensitive )
{
    std::string l_pattern(wildcardPattern);
    // Escape all regex special chars
    escapeRegex(l_pattern);

    // Convert chars '*?' back to their regex equivalents
    boost::replace_all(l_pattern, "\\?", ".");
    boost::replace_all(l_pattern, "\\*", ".*");

    boost::regex pattern(l_pattern,
                         caseSensitive ?
                             boost::regex::normal :
                             boost::regex::icase);

    return regex_match(text, pattern);
}

std::vector<std::string> zm::common::split(
        const std::string &input,
        const char *separators)
{
    std::vector<std::string> l_result;
    boost::split(l_result, input, boost::is_any_of(separators));
    return l_result;
}

std::set< std::string > zm::common::get_files_in_dir(
        const std::string &a_directory,
        const std::string &a_wildcardPattern)
{

    std::set< std::string > l_result;

    boost::filesystem::path l_folder(a_directory);

    if(!boost::filesystem::exists(l_folder))
    {
        return l_result;
    }

    boost::filesystem::directory_iterator l_end_itr;
    boost::filesystem::directory_iterator l_fs_itr( l_folder );

    for( ; l_fs_itr != l_end_itr; ++l_fs_itr )
    {
        // skip if not a file
        if( !boost::filesystem::is_regular_file( l_fs_itr->status() ) )
        {
            continue;
        }

        bool l_match = false;

        /// match against all provided patterns
        BOOST_FOREACH(const std::string&l_pattern,
                      zm::common::split(a_wildcardPattern, ";"))
        {
            if( zm::common::matchesWildcards(
                        l_fs_itr->path().filename().string(),
                        l_pattern ))
            {
                l_match = true;
                break;
            }
        }
        if(!l_match)
        {
            continue;
        }

        l_result.insert(l_fs_itr->path().string());
   }

    return l_result;
}

std::string zm::common::to_string(int value)
{
#   if defined(ANDROID)
        std::stringstream l_stream;
        l_stream << value;
        return l_stream.str();
#   else
        return std::to_string(value);
#   endif
}
