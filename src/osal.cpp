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

BOOL GetMyHostName(LPSTR pszBuffer, UINT nLen)
{
    BOOL ret;

    ret = FALSE;

    if (pszBuffer && nLen)
    {
        if ( gethostname(pszBuffer, nLen) == 0 )
            ret = TRUE;
        else
            *pszBuffer = '\0';
    }

    return ret;
}

#include <windows.h>
#include <Lmcons.h>

// http://stackoverflow.com/questions/11587426/get-current-username-in-c-on-windows
char username[UNLEN+1];
GetUserName(username, UNLEN+1);

#endif
