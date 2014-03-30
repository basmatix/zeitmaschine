/// -*- coding: utf-8 -*-
///
/// file: common.h
///
/// Copyright (C) 2014 Frans Fuerst
///

#pragma once
#ifndef ZM_TESTING_COMMON_H
#define ZM_TESTING_COMMON_H

#include <mm/zmCommon.h>
#include <string>
#include <boost/filesystem/operations.hpp>

class CleanFolder
{
    std::string m_folder;

    void cleanup()
    {
        if( boost::filesystem::exists( m_folder ) )
        {
            boost::filesystem::remove_all( m_folder );
        }
    }

    CleanFolder();

public:

    CleanFolder( const std::string &folder)
        : m_folder(folder)
    {cleanup(); }

    ~CleanFolder()
    {cleanup(); }

    operator std::string &()
    { return m_folder; }
};

int sync_folders(
        const std::string &a_source_path,
        const std::string &a_destination_path,
        const std::string &a_pattern = "*-journal.yaml;snapshot-*.yaml")
{
    boost::filesystem::path l_source_folder(a_source_path);
    boost::filesystem::path l_destination_folder(a_destination_path);

    std::vector<std::string> l_patternlist(
                zm::common::split(a_pattern, ";"));

    l_source_folder /= "sync";
    l_destination_folder /= "sync";

    if(!boost::filesystem::exists(l_source_folder))
    {
        return 0;
    }

    if(!boost::filesystem::exists(l_destination_folder))
    {
        boost::filesystem::create_directories(l_destination_folder);
    }

    boost::filesystem::directory_iterator l_end_itr;
    boost::filesystem::directory_iterator l_fs_itr( l_source_folder );

    int l_result = 0;

    for( ; l_fs_itr != l_end_itr; ++l_fs_itr )
    {
        // skip if not a file
        if( !boost::filesystem::is_regular_file( l_fs_itr->status() ) )
        {
            continue;
        }

        bool l_match = false;

        /// match against all provided patterns
        for(const std::string&l_pattern: l_patternlist)
        {
            printf("'%s' '%s'",
                   l_pattern.c_str(),
                   l_fs_itr->path().filename().string().c_str());fflush(stdout);

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


        boost::filesystem::path l_dest_file =
                l_destination_folder / l_fs_itr->path().filename();

        printf("copy '%s' to '%s'\n",
               l_fs_itr->path().string().c_str(),
               l_dest_file.string().c_str()); fflush(stdout);

        if( ! boost::filesystem::exists(l_dest_file))
        {
            zm::common::copy_file(
                        l_fs_itr->path().string(),
                        l_dest_file.string());
            l_result += 1;
        }
        else
        {
            // todo: check if same, else error
        }
    }

    return l_result;
}

#endif
