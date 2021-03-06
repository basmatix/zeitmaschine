# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (ysbox-unit_tests-common)
#
# Copyright 2014 Frans Fuerst
#

# test specific includes
#include_directories( "${CMAKE_SOURCE_DIR}/common/include" )

# general include directories
include_directories( SYSTEM "${Boost_INCLUDE_DIR}" )

include_directories( SYSTEM
    "${YAML_CPP_INCLUDE_DIR}"
)

include_directories(
    "${MM_INCLUDE_DIR}"
)

add_executable( test_mindmatter test_mindmatter.cpp )
target_link_libraries( test_mindmatter
    mm_s
    yaml-cpp
    ${common_libraries}
)

set( test_executable "${CMAKE_CURRENT_BINARY_DIR}/test_mindmatter" )

add_test( mm_low_level_gtd_workflow   ${test_executable} mm_low_level_gtd_workflow )
add_test( mm_connections              ${test_executable} mm_connections )
add_test( mm_empty_db_on_load         ${test_executable} mm_empty_db_on_load )
add_test( mm_diff_and_reapply         ${test_executable} mm_diff_and_reapply )
# deactivated because assumptions not valid anymore
#add_test( mm_things_change_while_open ${test_executable} mm_change_while_open )
add_test( mm_persist_and_load         ${test_executable} mm_persist_and_load )
add_test( mm_journaled_sync           ${test_executable} mm_journaled_sync )
add_test( mm_equality                 ${test_executable} mm_equality )
add_test( mm_snapshot                 ${test_executable} mm_snapshot )
add_test( mm_subfolders               ${test_executable} mm_subfolders )

