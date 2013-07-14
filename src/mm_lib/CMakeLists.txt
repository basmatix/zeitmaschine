# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (mindmatter)
#
# Copyright (C) 2013 Frans Fuerst
#

project( mindmatter )

cmake_minimum_required(VERSION 2.8.11)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.txt
    *.h
    DOC-*
    *asciidoc.txt
    zmOsal.cpp
    zmModel.cpp
    zmChangeSet.cpp
    zmCommon.cpp
    include/*/*.h
)

# generated and external headers made system so
# we don't have to deal with their warnings
include_directories(SYSTEM
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${YAML_CPP_INCLUDE_DIR}"
    "${Boost_INCLUDE_DIR}"
)

add_library( mm_s STATIC  ${${PROJECT_NAME}_files} )

set_target_properties( mm_s PROPERTIES COMPILE_FLAGS -fPIC )

link_directories( ${Boost_LIBRARY_DIRS} )
