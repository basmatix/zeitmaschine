# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (mindmatter)
#
# Copyright (C) 2013 Frans Fuerst
#

project( zm_tool )

cmake_minimum_required(VERSION 2.8.11)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.txt
    *.h
    DOC-*
    *asciidoc.txt
    zmTool.cpp
)

add_executable( zmTool ${${PROJECT_NAME}_files} )

target_link_libraries( zmTool
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    mm_s
    zm_s
    yaml-cpp
)
