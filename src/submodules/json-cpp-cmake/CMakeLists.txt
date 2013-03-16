project( json-cpp )

include_directories( "../json-cpp/include" )

file( GLOB ${PROJECT_NAME}_FILES
    "../json-cpp/src/lib_json/json_reader.cpp"
    "../json-cpp/src/lib_json/json_value.cpp"
    "../json-cpp/src/lib_json/json_writer.cpp"
)

add_library( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_FILES})

