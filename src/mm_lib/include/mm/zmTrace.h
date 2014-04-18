/// -*- coding: utf-8 -*-
///
/// file: zmTrace.h
///
/// Copyright (C) 2013 Frans Fuerst
///

#ifndef ZMTRACE_H
#define ZMTRACE_H

//0: off
//1: critical
//2: error
//3: warning
//4: info
//5: debug
//6: debug2

void set_trace_level( int level );

void trace_i( const char * a_format, ... );
void trace_d( const char * a_format, ... );
void trace_e( const char * a_format, ... );
void trace_w( const char * a_format, ... );

#if defined(DEBUG)
void assert_fail_hard(
        const char *assertion,
        const char *file,
        unsigned int line,
        const char *function);
void assert_fail_soft(
        const char *assertion,
        const char *file,
        unsigned int line,
        const char *function);
#   define trace_assert_s(expr)     \
      ((expr)                       \
       ? __ASSERT_VOID_CAST (0)     \
       : assert_fail_soft (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))
#   define trace_assert_h(expr)     \
      ((expr)                       \
       ? __ASSERT_VOID_CAST (0)     \
       : assert_fail_hard (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))
#else
#   define trace_assert_h(expr)
#   define trace_assert_s(expr)
#endif

#endif
