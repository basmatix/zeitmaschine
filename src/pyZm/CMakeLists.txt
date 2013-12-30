# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (pyZm)
#
# Copyright (C) 2013 Frans Fuerst
#

project( pyZm )

cmake_minimum_required(VERSION 2.8.11)

option( CONFIG_BUILD_PYZM "build Python module" ON )
if( CONFIG_BUILD_PYZM )

    file( GLOB ${PROJECT_NAME}_files
        CMakeLists.txt
        *.txt
        *.h
        DOC-*
        *asciidoc.txt
    )

    find_package( PythonLibs 2.6 REQUIRED )
    include_directories( ${PYTHON_INCLUDE_DIR} )
    get_filename_component( PYTHON_LIBRARY_DIR ${PYTHON_LIBRARY} PATH )
    link_directories( ${PYTHON_LIBRARY_DIR} )

    find_package( SWIG REQUIRED )
    include( ${SWIG_USE_FILE} )

    set(PYZM_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "" FORCE)

    include_directories(
        ${MM_INCLUDE_DIR}
        ${ZM_INCLUDE_DIR}
    )

    set( PYTHON_ZM_LIBRARY_NAME "pyZm" )

    set( SWIG_MODULE_${PYTHON_ZM_LIBRARY_NAME}_EXTRA_DEPS "${ZM_INCLUDE_DIR}/zm/zmGtdModel.h" )

    set_source_files_properties( pyZm.i PROPERTIES CPLUSPLUS ON )

    swig_add_module( ${PYTHON_ZM_LIBRARY_NAME} python pyZm.i )

    # disable Weffc++ for swig generated file
    if( NOT MSVC )
        set_source_files_properties(
            ${CMAKE_CURRENT_BINARY_DIR}/pyZmPYTHON_wrap.cxx
            PROPERTIES COMPILE_FLAGS -Wno-effc++
        )
    endif()

    # important: add _ to swig generated python modules
    #set_target_properties( _${PYTHON_ZM_LIBRARY_NAME} PROPERTIES COMPILE_DEFINITIONS "${YSBOX_COMPILE_FLAGS}" )

    swig_link_libraries( ${PYTHON_ZM_LIBRARY_NAME} zm_s mm_s yaml-cpp )

    if( NOT MSVC )
         swig_link_libraries( ${PYTHON_ZM_LIBRARY_NAME} ${Boost_LIBRARIES} )
    endif()

endif()
