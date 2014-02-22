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
void trace_e( const char * a_format, ... );

#endif
