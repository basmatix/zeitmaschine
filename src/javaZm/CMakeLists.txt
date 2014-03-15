# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (javaZm)
#
# Copyright (C) 2013 Frans Fuerst
#

project( javaZm )

cmake_minimum_required(VERSION 2.8.11)

option( CONFIG_BUILD_JAVAZM "build java module" ON )
if( CONFIG_BUILD_JAVAZM )

    file( GLOB ${PROJECT_NAME}_files
        CMakeLists.txt
        test-javaZm.java
        *.txt
        *.h
        DOC-*
        *asciidoc.txt
    )

    if(NOT ANDROID)
        #jni is available by default on android

        find_package( JNI REQUIRED )
        # does not work on MacOS - have another check
        if( NOT EXISTS "${JAVA_INCLUDE_PATH}/jni.h" )

            message( FATAL_ERROR "JNI not found. Probably you need to install Java SDK." )

            if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                message( WARNING "Running on Mac OS, you probably need Java OS X Developer package, too." )
            endif()

            add_custom_target( ${PROJECT_NAME}_DEACTIVATED SOURCES ${${PROJECT_NAME}_FILES})

        endif()

        include_directories(
            ${JNI_INCLUDE_DIRS}
        )
    endif()

    find_package( SWIG REQUIRED )
    include( ${SWIG_USE_FILE} )

    set(JAVAZM_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "" FORCE)

    include_directories(
        ${MM_INCLUDE_DIR}
        ${ZM_INCLUDE_DIR}
    )

    include_directories( SYSTEM "${Boost_INCLUDE_DIR}" )

    set( JAVA_ZM_LIBRARY_NAME "javaZm" )

    set( SWIG_MODULE_${JAVA_ZM_LIBRARY_NAME}_EXTRA_DEPS
        "${ZM_INCLUDE_DIR}/zm/zmGtdModel.h"
        "${MM_INCLUDE_DIR}/mm/zmModel.h" )

    set_source_files_properties( javaZm.i PROPERTIES CPLUSPLUS ON )

    swig_add_module( ${JAVA_ZM_LIBRARY_NAME} java javaZm.i )

    # disable Weffc++ for swig generated file
    if( NOT MSVC )
        set_source_files_properties(
            ${CMAKE_CURRENT_BINARY_DIR}/javaZmJAVA_wrap.cxx
            PROPERTIES COMPILE_FLAGS -Wno-effc++
        )
    endif()

    # important: add _ to swig generated python modules
    #set_target_properties( _${PYTHON_ZM_LIBRARY_NAME} PROPERTIES COMPILE_DEFINITIONS "${YSBOX_COMPILE_FLAGS}" )

    swig_link_libraries( ${JAVA_ZM_LIBRARY_NAME} zm_s mm_s yaml-cpp )

    get_target_property(TARGET_LOCATION ${JAVA_ZM_LIBRARY_NAME} LOCATION)
    message( WARNING "XXXXX ${TARGET_LOCATION}")
    if( NOT MSVC )
         swig_link_libraries( ${JAVA_ZM_LIBRARY_NAME} ${Boost_LIBRARIES} )
    endif()

endif()
