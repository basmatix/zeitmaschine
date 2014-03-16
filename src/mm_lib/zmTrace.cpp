/// -*- coding: utf-8 -*-
///
/// file: zmTrace.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

//0: off
//1: critical
//2: error
//3: warning
//4: info
//5: debug
//6: debug2

int g_trace_level = 2;

void set_trace_level( int level )
{
    g_trace_level = level;
}

#if defined(ANDROID)

#include <android/log.h>
#include <stdarg.h>
#include <stdio.h>

#define  LOG_TAG    "zm"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)

void trace_i( const char * a_format, ... )
{
    if(g_trace_level < 4) return;

    char l_buffer[1024];
    va_list l_args;
    va_start (l_args, a_format);
    vsnprintf ( l_buffer, 1023, a_format, l_args );
    va_end( l_args );
    ALOG("(II) %s", l_buffer );
}

void trace_e( const char * a_format, ... )
{
    if(g_trace_level < 2) return;

    char l_buffer[1024];
    va_list l_args;
    va_start (l_args, a_format);
    vsnprintf ( l_buffer, 1023, a_format, l_args );
    va_end( l_args );
    ALOG("(EE) %s", l_buffer );
    ALOG("(EE) at %s:%d", __FILE__, __LINE__);
}

#else

#include <stdarg.h>
#include <stdio.h>

void trace_i( const char * a_format, ... )
{
    if(g_trace_level < 4) return;

    char l_buffer[1024];
    va_list l_args;
    va_start (l_args, a_format);
    vsprintf ( l_buffer, a_format, l_args );
    va_end( l_args );
    printf( "%s\n", l_buffer );
    fflush( stdout );
}

void trace_e( const char * a_format, ... )
{
    if(g_trace_level < 2) return;

    char l_buffer[1024];
    va_list l_args;
    va_start (l_args, a_format);
    vsprintf ( l_buffer, a_format, l_args );
    va_end( l_args );
    printf( "%s\n", l_buffer );
    fflush( stdout );
}
#endif


