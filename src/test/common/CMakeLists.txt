# -*- coding: utf-8 -*-
#
# file: CMakeLists.txt (zeitmaschine-unit_tests-gtd)
#
# Copyright 2014 Frans Fuerst
#

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

