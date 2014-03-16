# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (zeitmaschine-unit_tests-gtd)
#
# Copyright 2011 - 2013 scitics GmbH
#
# Information  contained  herein  is  subject  to change  without  notice.
# scitics GmbH  retains ownership and  all other rights  in this software.
# Any reproduction of the software or components thereof without the prior
# written permission of scitics GmbH is prohibited.

# test specific includes
#include_directories( "${CMAKE_SOURCE_DIR}/common/include" )

# general include directories
include_directories( SYSTEM "${Boost_INCLUDE_DIR}" )

include_directories( SYSTEM
    "${YAML_CPP_INCLUDE_DIR}"
)

include_directories(
    "${MM_INCLUDE_DIR}"
    "${ZM_INCLUDE_DIR}"
)

add_executable( test_gtd test_gtd.cpp )
target_link_libraries( test_gtd
    mm_s
    zm_s
    yaml-cpp
    ${common_libraries}
)

set( test_executable "${CMAKE_CURRENT_BINARY_DIR}/test_gtd" )

add_test( gtd_basic_workflow    ${test_executable} gtd_basic_workflow )

