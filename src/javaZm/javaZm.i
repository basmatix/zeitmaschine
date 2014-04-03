/// -*- coding: utf-8 -*-
///
/// file: zm.i
///
/// Copyright (C) 2013 Frans Fuerst
///


%module javaZm

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "boost_shared_ptr.i"

//%array_class(int, intArray);
//%array_class(double, doubleArray);
//%array_class(char, charArray);
//%array_class(char, charArray);

%shared_ptr(zm::MindMatterModel)

%{
// includes the header in the wrapper code

#include <zm/zmGtdModel.h>
#include <mm/zmModel.h>
#include <mm/zmChangeSet.h>
%}

//%template()         std::vector<std::string>;
%template(StringVec) std::vector<std::string>;
//%template()         std::list<std::string>;

// Parse the header file to generate wrappers
%include <zm/zmGtdModel.h>
%include <mm/zmTypes.h>
%include <mm/zmModel.h>
%include <mm/zmChangeSet.h>
