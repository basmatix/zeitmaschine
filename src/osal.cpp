/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#include "osal.h"

#ifdef __unix__

#include <unistd.h>

std::string osal::getHostName()
{
    char l_hostname[1024];
    gethostname(l_hostname,1024);
    return std::string( l_hostname );
}

std::string osal::getUserName()
{
    return std::string( getlogin() );
}

#endif

#ifdef WIN32
#endif
