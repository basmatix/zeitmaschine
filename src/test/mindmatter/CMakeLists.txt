# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (ysbox-unit_tests-common)
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
include_directories( "${Boost_INCLUDE_DIR}" )
link_directories( ${Boost_LIBRARY_DIRS} )

include_directories( SYSTEM
    "${YAML_CPP_INCLUDE_DIR}"
)

include_directories(
    "${MM_INCLUDE_DIR}"
)

add_executable( test_mindmatter test_mindmatter.cpp )
target_link_libraries( test_mindmatter
    ${Boost_LIBRARIES}
    mm_s
    zm_s
    yaml-cpp
)

set( test_executable "${CMAKE_CURRENT_BINARY_DIR}/test_mindmatter" )

add_test( empty_db_on_load         ${test_executable} empty_db_on_load )
add_test( things_change_while_open ${test_executable} change_while_open )
add_test( output_node              ${test_executable} output_node )
