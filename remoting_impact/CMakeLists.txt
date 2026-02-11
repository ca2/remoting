cmake_minimum_required(VERSION 3.16)
project(remoting_impact)



list(APPEND executable_source
   framework.cpp
   framework.h
   ViewerCollector.cpp
   OptionsDialog.cpp
   ConnectionListener.h
   AuthenticationDialog.h
   ScaleManager.h
   DesktopWindow.cpp
   FileRenameDialog.h
   HelpDialog.cpp
   LoginDialog.h
   ConnectionListener.cpp
   FileInfoListView.cpp
   ViewerCmdLine.h
   ViewerWindow.h
   ViewerMenu.cpp
   BuildTime.h
   FileTransferMainDialog.h
   DesktopWindow.h
   FileTransferMainDialog.cpp
   HelpDialog.h
   AuthenticationDialog.cpp
   ResourceStrings.h
   AboutDialog.h
   FsWarningDialog.h
   NamingDefs.h
   ScaleManager.cpp
   ConfigurationDialog.cpp
   BuildTime.cpp
   main.cpp
   ConfigurationDialog.h
   ViewerCollector.h
   FileExistDialog.cpp
   ViewerCmdLine.cpp
   ControlTrayIcon.cpp
   FileInfoListView.h
   ViewerWindow.cpp
   OptionsDialog.h
   remoting_impact.cpp
   ConnectionData.h
   FileRenameDialog.cpp
   NamingDefs.cpp
   ViewerInstance.h
   FsWarningDialog.cpp
   ConnectionData.cpp
   NewFolderDialog.cpp
   remoting_impact.h
   LoginDialog.cpp
   ViewerMenu.h
   ViewerVncAuthHandler.h
   ResourceStrings.cpp
   FileExistDialog.h
   ViewerInstance.cpp
   AboutDialog.cpp
   ControlTrayIcon.h
   ViewerVncAuthHandler.cpp
   NewFolderDialog.h
   _.h
   _.cpp
)


list(APPEND executable_references
   acme
   remoting_network
   remoting_thread
   remoting_util
   remoting_viewer_core
   remoting_libjpeg
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_executable(${PROJECT_NAME} ${executable_source})






#target_link_directories(${PROJECT_NAME} PRIVATE ${executable_link_directories})

target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
#target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})
   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${executable_references} ${global_library_references})

elseif(${LINUX} OR ${__BSD__} OR ${ANDROID})
   target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive  ${EXTRA_CXX_TARGET_COMPILER_OPTIONS}>)
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-rpath=$ORIGIN)
   #target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)

endif ()

add_dependencies(${PROJECT_NAME} acme)
add_dependencies(${PROJECT_NAME} remoting_libjpeg)
add_dependencies(${PROJECT_NAME} remoting_zlib)
add_dependencies(${PROJECT_NAME} remoting_network)
add_dependencies(${PROJECT_NAME} remoting_thread)
add_dependencies(${PROJECT_NAME} remoting_util)
add_dependencies(${PROJECT_NAME} remoting_viewer_core)


list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${executable_include_directories})

