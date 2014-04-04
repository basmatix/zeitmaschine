project( check_header_dependencies )

file( GLOB ${PROJECT_NAME}_FILES generated/*.cpp )

# include all needed directories
include_directories(
    "${CMAKE_SOURCE_DIR}/mm_lib/include"
    "${CMAKE_SOURCE_DIR}/zm_lib/include"
)

include_directories( SYSTEM "${Boost_INCLUDE_DIR}" )

# create a static library to avoid linker problems
add_library( ${PROJECT_NAME} STATIC
    dummy.cpp
    ${${PROJECT_NAME}_FILES}
)

#if(NOT MSVC)
#    set_target_properties( ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -ftime-report )
#endif()
