cmake_minimum_required(VERSION 3.16)
project(remoting_ft_common)


list(APPEND library_source
   framework.cpp
   framework.h
   WinFilePath.h
   WinFilePath.cpp
   OperationNotSupportedException.h
   OperationNotSupportedException.cpp
   FTMessage.h
   FTMessage.cpp
   FolderListener.h
   FileInfo.cpp
   FileInfo.h
   FolderListener.cpp
   FileTransferException.h
   FileTransferException.cpp
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

