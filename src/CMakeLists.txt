# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (zeitmaschine/root)
#
# Copyright (C) 2014 Frans Fuerst
#

project( zeitmaschine )

cmake_minimum_required(VERSION 2.8)

if(ANDROID)
    list(APPEND common_libraries log)
    message(STATUS "This is an Android build")
    include_directories("${ANDROID_NDK_TOOLCHAIN_ROOT}/include/c++/4.8")
    include_directories("${ANDROID_NDK_TOOLCHAIN_ROOT}/include/c++/4.8/arm-linux-androideabi")
    #link_directories( "${ANDROID_NDK_TOOLCHAIN_ROOT}/sysroot/usr/lib/" )

    set( Boost_USE_STATIC_LIBS   ON )  # shared boost libs -otherwise we would have to recompile boost with -fPIC
    #add_definitions( -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK )
    #set( Boost_USE_MULTITHREADED     ON  )  # better it is..
    #set( Boost_USE_STATIC_RUNTIME    ON  )  # don't have the system runtimes linked
    set(Boost_COMPILER -gcc)
    find_host_package(
            Boost 1.47
        COMPONENTS
            filesystem
            #serialization
            system
            #program_options
            #thread
            #iostreams
            date_time
            regex
        REQUIRED
    )

    # won't help
    #    set(SWIG_EXECUTABLE "/usr/bin/swig" )
    #    set(SWIG_DIR "/usr/share/swig/2.0.11")

else()
    set( Boost_USE_STATIC_LIBS   ON )  # shared boost libs -otherwise we would have to recompile boost with -fPIC
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
            #serialization
            system
            #program_options
            #thread
            #iostreams
            date_time
            regex
            filesystem
        REQUIRED
      )

endif()

set( YAML_CPP_BUILD_TOOLS OFF)
set( YAML_CPP_BUILD_CONTRIB OFF)

link_directories( ${Boost_LIBRARY_DIRS} )

list(APPEND common_libraries ${Boost_LIBRARIES})


if( NOT MSVC )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif()

option( CONFIG_CPP11 "compile with c++11 options" ON )
if( CONFIG_CPP11 )
    if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
        set( CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libstdc++" )
        set( CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -std=c++11 -stdlib=libstdc++" )
    elseif(ANDROID)
        # this is dirty - should abort

        #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGXX_EXPERIMENTAL_CXX0X" )

        set( CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -std=c++11" )
        set( CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -std=c++11" )

    elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU )
        include( CheckCXXCompilerFlag )
        check_cxx_compiler_flag(-std=c++11 HAVE_STD11)
        if(HAVE_STD11)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        else()
            check_cxx_compiler_flag(-std=c++0x HAVE_STD0X)
            if(HAVE_STD0X)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++0x")
            endif()
        endif()
    endif()
endif()

if( NOT CMAKE_CONFIGURATION_TYPES )
    # Build Debug by default
    if( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE )
    endif()
else()
    # Using a multi-configuration generator eg MSVC or Xcode
    # that uses CMAKE_CONFIGURATION_TYPES and not CMAKE_BUILD_TYPE
endif()

# http://www.robertnitsch.de/notes/cpp/cpp11_boost_filesystem_undefined_reference_copy_file
add_definitions( -DBOOST_NO_SCOPED_ENUMS -DBOOST_NO_CXX11_SCOPED_ENUMS )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions( -DDEBUG -D_DEBUG )
else()
    add_definitions( -DNDEBUG )
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

file( GLOB ${PROJECT_NAME}_files
    CMakeLists.txt
    DOC-*
    *asciidoc.txt
    *.svg
)

add_custom_target( ${PROJECT_NAME} SOURCES ${${PROJECT_NAME}_files} )

set( YAML_CPP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules/yaml-cpp/include" )
set( MM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mm_lib/include" )
set( ZM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/zm_lib/include" )

enable_testing()

add_subdirectory( submodules/yaml-cpp )
set_target_properties( yaml-cpp PROPERTIES POSITION_INDEPENDENT_CODE TRUE )

add_subdirectory( mm_lib )

add_subdirectory( zm_lib )

add_subdirectory( javaZm )

if(NOT ANDROID)
    #add_subdirectory( zmQt )
    add_subdirectory( zmQtListWidget )

    #add_subdirectory( zm_qt_mvc_stub )

    add_subdirectory( zm_tool )

    add_subdirectory( pyZm )

    add_subdirectory( test )
endif()

add_subdirectory( test/header_dependency_check )

if(CONFIG_BUILD_PYZM)
    option(CONFIG_HAVE_ASCIITEST "let asciidoc with asciitest create docs and tests" ON)
else()
    option(CONFIG_HAVE_ASCIITEST "let asciidoc with asciitest create docs and tests" OFF)
endif()
if(CONFIG_HAVE_ASCIITEST)
add_subdirectory( submodules/asciitest/asciitest )
    run_env_add_variable("PYTHONPATH" "${PYZM_BUILD_DIR}")
    #run_env_add_variable("PYTHONPATH" "${ASCIITEST_CONFIG_DIR}")
endif()
