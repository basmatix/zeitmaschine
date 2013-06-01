/// -*- coding: utf-8 -*-
///
/// file: zm.i
///
/// Copyright 2011 - 2013 scitics GmbH
///
/// Information  contained  herein  is  subject  to change  without  notice.
/// scitics GmbH  retains ownership and  all other rights  in this software.
/// Any reproduction of the software or components thereof without the prior
/// written permission of scitics GmbH is prohibited.

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
