cmake_minimum_required(VERSION 3.16)
project(remoting_remoting)


list(APPEND library_source

   impact_toolbar.cpp
   impact_toolbar.h
   application.cpp
   application.h
   AboutDialog.cpp
   AboutDialog.h
   AuthenticationDialog.cpp
   AuthenticationDialog.h
   BuildTime.cpp
   BuildTime.h
   ConfigurationDialog.cpp
   ConfigurationDialog.h
   ConnectingDialog.cpp
   ConnectingDialog.h
   ConnectionData.cpp
   ConnectionData.h
   ConnectionListener.cpp
   ConnectionListener.h
   ControlTrayIcon.cpp
   ControlTrayIcon.h
   DesktopWindow.cpp
   DesktopWindow.h
   FileExistDialog.cpp
   FileExistDialog.h
   FileInfoListView.cpp
   FileInfoListView.h
   FileRenameDialog.cpp
   FileRenameDialog.h
   FileTransferMainDialog.cpp
   FileTransferMainDialog.h
   FsWarningDialog.cpp
   FsWarningDialog.h
   HelpDialog.cpp
   HelpDialog.h
   LoginDialog.cpp
   LoginDialog.h
   NamingDefs.cpp
   NamingDefs.h
   NewFolderDialog.cpp
   NewFolderDialog.h
   OptionsDialog.cpp
   OptionsDialog.h
   ResourceStrings.cpp
   ResourceStrings.h
   ScaleManager.cpp
   ScaleManager.h
   ViewerCmdLine.cpp
   ViewerCmdLine.h
   ViewerCollector.cpp
   ViewerCollector.h
   ViewerInstance.cpp
   ViewerInstance.h
   ViewerMenu.cpp
   ViewerMenu.h
   ViewerVncAuthHandler.cpp
   ViewerVncAuthHandler.h
   ViewerWindow.cpp
   ViewerWindow.h
   _.cpp
   _.h
   framework.cpp
   framework.h
   application_main.cpp
   remoting_impact.cpp
   remoting_impact.h
   remoting_remoting.rc
)


list(APPEND library_references
   acme
   remoting_client_config_lib
   remoting_common
   remoting_config_lib
   #remoting_file_lib
   remoting_ftp_client
   remoting_ftp_common
   remoting_gui
   remoting_io_lib
   remoting_libjpeg
   #remoting_log_writer
   remoting_network
   remoting_region
   remoting_rfb
   remoting_thread
   remoting_util
   remoting_viewer_core
   remoting_win_system
   remoting_zlib
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(${PROJECT_NAME} SHARED ${library_source})






#target_link_directories(${PROJECT_NAME} PRIVATE ${executable_link_directories})

target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
#target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})
   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE Ws2_32 Version Comctl32 ${library_references} ${global_library_references})

elseif(${LINUX} OR ${__BSD__} OR ${ANDROID})
   target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive  ${EXTRA_CXX_TARGET_COMPILER_OPTIONS}>)
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-rpath=$ORIGIN)
   #target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)

endif ()

add_dependencies(${PROJECT_NAME} acme)
add_dependencies(${PROJECT_NAME} remoting_common)
add_dependencies(${PROJECT_NAME} remoting_client_config_lib)
add_dependencies(${PROJECT_NAME} remoting_config_lib)
#add_dependencies(${PROJECT_NAME} remoting_file_lib)
add_dependencies(${PROJECT_NAME} remoting_ftp_common)
add_dependencies(${PROJECT_NAME} remoting_ftp_client)
add_dependencies(${PROJECT_NAME} remoting_gui)
add_dependencies(${PROJECT_NAME} remoting_io_lib)
add_dependencies(${PROJECT_NAME} remoting_libjpeg)
#add_dependencies(${PROJECT_NAME} remoting_log_writer)
add_dependencies(${PROJECT_NAME} remoting_zlib)
add_dependencies(${PROJECT_NAME} remoting_network)
add_dependencies(${PROJECT_NAME} remoting_region)
add_dependencies(${PROJECT_NAME} remoting_rfb)
add_dependencies(${PROJECT_NAME} remoting_thread)
add_dependencies(${PROJECT_NAME} remoting_util)
add_dependencies(${PROJECT_NAME} remoting_viewer_core)
add_dependencies(${PROJECT_NAME} remoting_win_system)
add_dependencies(${PROJECT_NAME} remoting_zlib)


list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${executable_include_directories})

add_subdirectory(__implement)