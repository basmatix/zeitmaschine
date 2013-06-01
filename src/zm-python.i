/// -*- coding: utf-8 -*-
///
/// file: zm.i
///
/// Copyright (C) 2013 Frans Fuerst
///


%module pyZm
%{
// Includes the header in the wrapper code
#include <zmGtdModel.h>
%}

// Parse the header file to generate wrappers
//#define OUTCHAR char
//%include "std_string.i"
//%include "typemaps.i"
//%include "carrays.i"
//%array_class(int, intArray);
//%array_class(double, doubleArray);
//%array_class(char, charArray);
//%include "cstring.i"

%include "zmGtdModel.h"
