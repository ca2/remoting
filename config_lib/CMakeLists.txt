cmake_minimum_required(VERSION 3.16)
project(remoting_config_lib)





list(APPEND library_source
   framework.cpp
   framework.h
   _.h
   SettingsManager.cpp
   IniFileSettingsManager.cpp
   IniFileSettingsManager.h
   RegistrySettingsManager.h
   ConnectionHistory.h
   ConnectionHistory.cpp
   _.cpp
   SettingsManager.h
   RegistrySettingsManager.cpp
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



