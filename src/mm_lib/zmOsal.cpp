/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#include <mm/zmOsal.h>

#ifdef __unix__

//#include <unistd.h>
#include <cstdlib>

std::string zm::osal::getHostName()
{
    // not available in android
    //char l_hostname[1024];
    //gethostname(l_hostname, 1024);

    return std::string(std::getenv("HOSTNAME"));
}

std::string zm::osal::getUserName()
{
    // not available in android
    //return std::string( getlogin() );

    // todo test if this should be USERNAME
    return std::string(std::getenv("USER"));
}

std::string zm::osal::getHomePath()
{
    return std::string(std::getenv("HOME"));
}

#endif

#ifdef WIN32

std::string zm::osal::getHostName()
{
    return "";
}
/*
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
*/
#include <windows.h>
#include <Lmcons.h>

std::string zm::osal::getUserName()
{
    return "";
}

std::string zm::osal::getHomePath()
{
    return std::string() + getenv("HOMEDRIVE") + getenv("HOMEPATH");
}

/*
// http://stackoverflow.com/questions/11587426/get-current-username-in-c-on-windows
char username[UNLEN+1];
GetUserName(username, UNLEN+1);
*/
#endif
