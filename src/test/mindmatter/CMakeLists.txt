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
include_directories( "${CMAKE_SOURCE_DIR}/common/include" )

# general include directories
include_directories( "${Boost_INCLUDE_DIR}" )
link_directories( ${Boost_LIBRARY_DIRS} )

add_executable( test_common test_common.cpp )
target_link_libraries( test_common common )

set( test_executable "${CMAKE_CURRENT_BINARY_DIR}/test_common" )

add_test( common_get_file_owner  ${test_executable} common_get_file_owner )
add_test( common_get_file_group  ${test_executable} common_get_file_group )
add_test( common_get_file_size   ${test_executable} common_get_file_size )
