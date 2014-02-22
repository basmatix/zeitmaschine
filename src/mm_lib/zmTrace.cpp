/// -*- coding: utf-8 -*-
///
/// file: zmTrace.cpp
///
/// Copyright (C) 2013 Frans Fuerst
///

#include <stdarg.h>
#include <stdio.h>

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
