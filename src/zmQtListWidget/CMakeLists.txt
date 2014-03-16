# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (zmQt)
#
# Copyright (C) 2013 Frans Fuerst
#

project( zmQt )

cmake_minimum_required(VERSION 2.8)

find_package( Qt4 REQUIRED )
#SET( QT_USE_QTXML TRUE)
#SET( QT_USE_QTNETWORK TRUE)
include( ${QT_USE_FILE} )

qt4_wrap_ui( ${PROJECT_NAME}_UIS zmQtListWidget.ui )

# list all class files which implement Q_OBJECT
qt4_wrap_cpp( ${PROJECT_NAME}_MOCS
    zmQTreeWidget.h
    zmQtUi.h
)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.txt
    *.h
    DOC-*
    *asciidoc.txt
    zmQtMain.cpp
    zmQtUi.cpp
    zmQtGtdModel.cpp
)

# generated and external headers made system so
# we don't have to deal with their warnings
include_directories(SYSTEM
    "${Boost_INCLUDE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${MM_INCLUDE_DIR}"
    "${ZM_INCLUDE_DIR}"
)

link_directories( ${Boost_LIBRARY_DIRS} )

add_executable( zm
    ${${PROJECT_NAME}_files}
    ${${PROJECT_NAME}_MOCS}
    ${${PROJECT_NAME}_UIS}
)

#add_custom_target( resettimestamp ALL
#    COMMAND touch "${CMAKE_CURRENT_SOURCE_DIR}/zmQtUi.h"
#)

target_link_libraries( zm
    ${QT_LIBRARIES}
    mm_s
    zm_s
    yaml-cpp
    ${Boost_LIBRARIES}
)
