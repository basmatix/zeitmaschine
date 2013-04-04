# copyright (C) 2013 Frans Fürst
# -*- coding: utf-8 -*-

project( zeitmaschine )

cmake_minimum_required(VERSION 2.8)

enable_testing()

find_package(
        Boost 1.47
    COMPONENTS
        filesystem
        serialization
        system
        thread
        program_options
        iostreams
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
    mainwindow.h
    zmQTreeWidget.h
)

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    *.h
    main.cpp
)

add_subdirectory( submodules/yaml-cpp )

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    #"${CMAKE_CURRENT_SOURCE_DIR}/submodules/json-cpp/include"
)

# generated and external headers made system so
# we don't have to deal with their warnings
include_directories(SYSTEM
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/submodules/yaml-cpp/include"
)

add_executable( zm
    ${${PROJECT_NAME}_files}
    ${${PROJECT_NAME}_MOCS}
    ${${PROJECT_NAME}_UIS}
)

target_link_libraries( zm
    ${QT_LIBRARIES}
    ${ADDITIONAL_LIBRARIES}
    ${Boost_LIBRARIES}
    #yaml-cppjson-cpp
    yaml-cpp
)

add_executable( zmTool zmTool.cpp )
target_link_libraries( zmTool
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    yaml-cpp
)

add_executable( test_things test/test_things.cpp )

target_link_libraries( test_things
    ${Boost_LIBRARIES}
    yaml-cpp
)

add_test( empty-db-on-load         ${CMAKE_CURRENT_BINARY_DIR}/test_things empty-db-on-load )
add_test( things-change-while-open ${CMAKE_CURRENT_BINARY_DIR}/test_things change-while-open )
add_test( output-node              ${CMAKE_CURRENT_BINARY_DIR}/test_things output-node )

