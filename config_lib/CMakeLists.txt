cmake_minimum_required(VERSION 3.16)
project(remoting_config_lib)





list(APPEND library_source
   framework.cpp
   framework.h
)


list(APPEND library_references
   acme
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(${PROJECT_NAME} STATIC ${library_source})



