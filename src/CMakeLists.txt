# -*- coding: utf-8 -*-
#
# file: zmQtGtdModel.h
#
# Copyright (C) 2013 Frans Fuerst
#

project( zeitmaschine )

cmake_minimum_required(VERSION 2.8)

enable_testing()

#if( WIN32 )
#    set( Boost_USE_STATIC_LIBS   ON  )  # on windows shared libraries seem to be not very well supported - diable them
#else()
#    set( Boost_USE_STATIC_LIBS   OFF )  # shared boost libs -otherwise we would have to recompile boost with -fPIC
#    add_definitions( -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )
#endif()
#set( Boost_FIND_REQUIRED TRUE)
#set( Boost_USE_MULTITHREADED     ON  )  # better it is..
#set( Boost_USE_STATIC_RUNTIME    ON  )  # don't have the system runtimes linked

find_package(
        Boost 1.47
    COMPONENTS
        filesystem
        #serialization
        system
        #thread
        program_options
        #iostreams
        date_time
        regex
    REQUIRED
  )

find_package( Qt4 REQUIRED )
#SET( QT_USE_QTXML TRUE)
#SET( QT_USE_QTNETWORK TRUE)
include( ${QT_USE_FILE} )

if( NOT CMAKE_CONFIGURATION_TYPES )
    # Build Debug by default
    if( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE )
    endif()
else()
    # Using a multi-configuration generator eg MSVC or Xcode
    # that uses CMAKE_CONFIGURATION_TYPES and not CMAKE_BUILD_TYPE
endif()

if( NOT MSVC )
    option( CONFIG_WEFFCPP "set warning level for real developers" ON )
    if( CONFIG_WEFFCPP )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-extra-args" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type" )
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wparentheses" )

         #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic" )
         #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare" )
         # does not exist on older versions of gcc
         #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-but-set-variable" )
    endif()
endif()

qt4_wrap_ui( ${PROJECT_NAME}_UIS
    zeitmaschine.ui
)

# list all class files which implement Q_OBJECT
qt4_wrap_cpp( ${PROJECT_NAME}_MOCS
    zmQtUi.h
    #zmQTreeWidget.h
)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.txt
    *.h
    DOC-*
    *asciidoc.txt
    main.cpp
    zmQtGtdItem.cpp
    zmQtUi.cpp
    zmQtGtdModel.cpp
)

set( YAML_CPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules/yaml-cpp/include" )

add_subdirectory( submodules/yaml-cpp )
add_subdirectory( mm_lib )

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    #"${CMAKE_CURRENT_SOURCE_DIR}/submodules/json-cpp/include"
)

# generated and external headers made system so
# we don't have to deal with their warnings
include_directories(SYSTEM
    "${Boost_INCLUDE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/submodules/yaml-cpp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/mm_lib/include"
)

add_library( zm_s STATIC
    zmGtdModel.cpp
)

set_target_properties( zm_s PROPERTIES COMPILE_FLAGS -fPIC )

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
    ${Boost_LIBRARIES}
    mm_s
    zm_s
    yaml-cpp
)

add_executable( zmTool
    zmTool.cpp
    zmQtGtdItem.cpp
    zmGtdModel.cpp
    zmQtGtdModel.cpp
)

target_link_libraries( zmTool
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    mm_s
    zm_s
    yaml-cpp
)

find_package( PythonLibs 2.6 REQUIRED )
include_directories( ${PYTHON_INCLUDE_DIR} )
get_filename_component( PYTHON_LIBRARY_DIR ${PYTHON_LIBRARY} PATH )
link_directories( ${PYTHON_LIBRARY_DIR} )

find_package( SWIG REQUIRED )
include( ${SWIG_USE_FILE} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PYTHON_ZM_LIBRARY_NAME "pyZm" )

set( SWIG_MODULE_${PYTHON_ZM_LIBRARY_NAME}_EXTRA_DEPS "zmGtdModel.h" )

set_source_files_properties( zm-python.i PROPERTIES CPLUSPLUS ON )

swig_add_module( ${PYTHON_ZM_LIBRARY_NAME} python zm-python.i )

# important: add _ to swig generated python modules
#set_target_properties( _${PYTHON_ZM_LIBRARY_NAME} PROPERTIES COMPILE_DEFINITIONS "${YSBOX_COMPILE_FLAGS}" )

swig_link_libraries( ${PYTHON_ZM_LIBRARY_NAME}  mm_s zm_s yaml-cpp )

if( NOT MSVC )
     swig_link_libraries( ${PYTHON_ZM_LIBRARY_NAME} ${Boost_LIBRARIES} )
endif()

add_subdirectory( test )


