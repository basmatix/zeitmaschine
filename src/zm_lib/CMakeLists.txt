# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (zeitmaschine)
#
# Copyright (C) 2013 Frans Fuerst
#

project( zeitmaschine )

cmake_minimum_required(VERSION 2.8.11)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.txt
    *.h
    DOC-*
    *asciidoc.txt
    zmGtdModel.cpp
    include/*/*.h
)

# generated and external headers made system so
# we don't have to deal with their warnings
include_directories(SYSTEM
    "${YAML_CPP_INCLUDE_DIR}"
    "${Boost_INCLUDE_DIR}"
)

include_directories(
    "${MM_INCLUDE_DIR}"
)

add_library( zm_s STATIC  ${${PROJECT_NAME}_files} )

set_target_properties( zm_s PROPERTIES COMPILE_FLAGS -fPIC )

link_directories( ${Boost_LIBRARY_DIRS} )
