cmake_minimum_required(VERSION 3.16)
project(remoting_log_writer)




list(APPEND library_source
   framework.cpp
   framework.h
   resource.h
   ProfileLogger.h
   ProfileLogger.cpp
   LogWriter.h
   LogWriter.cpp
   Logger.h
   LogDump.h
   LogDump.cpp
   FileLogger.h
   FileLogger.cpp
   FileAccount.cpp
   FileAccount.h
   _.h
   _.cpp
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



list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})



