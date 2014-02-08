/// -*- coding: utf-8 -*-
///
/// file: zm.i
///
/// Copyright (C) 2013 Frans Fuerst
///


%module pyZm
%{
// includes the header in the wrapper code

#include <zm/zmGtdModel.h>
#include <mm/zmModel.h>
#include <mm/zmChangeSet.h>
%}

// Parse the header file to generate wrappers
//#define OUTCHAR char
%include "std_string.i"
//%include "typemaps.i"
//%include "carrays.i"
//%array_class(int, intArray);
//%array_class(double, doubleArray);
//%array_class(char, charArray);
//%include "cstring.i"

%include <zm/zmGtdModel.h>
%include <mm/zmTypes.h>
%include <mm/zmModel.h>
%include <mm/zmChangeSet.h>
