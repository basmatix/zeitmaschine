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

project( test_common )

# general include directories
include_directories( SYSTEM
    "${Boost_INCLUDE_DIR}"
    "${YAML_CPP_INCLUDE_DIR}"
)

# test specific includes
include_directories(
    "${MM_INCLUDE_DIR}"
)

add_executable( ${PROJECT_NAME} ${PROJECT_NAME}.cpp )

target_link_libraries( ${PROJECT_NAME}
    mm_s
    ${common_libraries}
)

get_target_property(TARGET_LOCATION ${PROJECT_NAME} LOCATION)

add_test( common_options_basic     ${TARGET_LOCATION}  common_options_basic )

