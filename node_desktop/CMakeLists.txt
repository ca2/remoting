cmake_minimum_required(VERSION 3.16)
project(remoting_node_desktop)


list(APPEND library_source
   control_desktop/_.cpp
   control_desktop/_.h
   ##control_desktop/_____debug.xcconfig
   ##control_desktop/_____release.xcconfig
   control_desktop/AboutDialog.cpp
   control_desktop/AboutDialog.h
   control_desktop/ConnectCommand.cpp
   control_desktop/ConnectCommand.h
   control_desktop/ConnectStringParser.cpp
   control_desktop/ConnectStringParser.h
   control_desktop/ControlApplication.cpp
   control_desktop/ControlApplication.h
   control_desktop/ControlAuth.cpp
   control_desktop/ControlAuth.h
   control_desktop/ControlAuthDialog.cpp
   control_desktop/ControlAuthDialog.h
   control_desktop/ControlAuthException.cpp
   control_desktop/ControlAuthException.h
   control_desktop/ControlCommand.cpp
   control_desktop/ControlCommand.h
   control_desktop/ControlCommandLine.cpp
   control_desktop/ControlCommandLine.h
   control_desktop/ControlGate.cpp
   control_desktop/ControlGate.h
   control_desktop/ControlMessage.cpp
   control_desktop/ControlMessage.h
   control_desktop/ControlPipeName.cpp
   control_desktop/ControlPipeName.h
   control_desktop/ControlProto.h
   control_desktop/ControlProxy.cpp
   control_desktop/ControlProxy.h
   control_desktop/ControlTrayIcon.cpp
   control_desktop/ControlTrayIcon.h
   control_desktop/DisconnectAllCommand.cpp
   control_desktop/DisconnectAllCommand.h
   ##control_desktop/framework.cpp
   ##control_desktop/framework.h
   control_desktop/MakeRfbConnectionCommand.cpp
   control_desktop/MakeRfbConnectionCommand.h
   #control_desktop/matter.txt
   control_desktop/NamedPipeTransport.cpp
   control_desktop/NamedPipeTransport.h
   control_desktop/Notificator.cpp
   control_desktop/Notificator.h
   control_desktop/OutgoingConnectionDialog.cpp
   control_desktop/OutgoingConnectionDialog.h
   control_desktop/ReloadConfigCommand.cpp
   control_desktop/ReloadConfigCommand.h
   control_desktop/RemoteException.cpp
   control_desktop/RemoteException.h
   control_desktop/resource.h
   control_desktop/RfbClientInfo.cpp
   control_desktop/RfbClientInfo.h
   control_desktop/ServerInfo.h
   control_desktop/SetPasswordsDialog.cpp
   control_desktop/SetPasswordsDialog.h
   control_desktop/ShareAppCommand.cpp
   control_desktop/ShareAppCommand.h
   control_desktop/ShareDisplayCommand.cpp
   control_desktop/ShareDisplayCommand.h
   control_desktop/ShareFullCommand.cpp
   control_desktop/ShareFullCommand.h
   control_desktop/SharePrimaryCommand.cpp
   control_desktop/SharePrimaryCommand.h
   control_desktop/ShareRectCommand.cpp
   control_desktop/ShareRectCommand.h
   control_desktop/ShareWindowCommand.cpp
   control_desktop/ShareWindowCommand.h
   control_desktop/ShutdownCommand.cpp
   control_desktop/ShutdownCommand.h
   control_desktop/SocketIPv4Transport.cpp
   control_desktop/SocketIPv4Transport.h
   control_desktop/Transport.cpp
   control_desktop/Transport.h
   control_desktop/TransportFactory.cpp
   control_desktop/TransportFactory.h
   control_desktop/UpdateLocalConfigCommand.cpp
   control_desktop/UpdateLocalConfigCommand.h
   control_desktop/UpdateRemoteConfigCommand.cpp
   control_desktop/UpdateRemoteConfigCommand.h
   AdditionalActionApplication.cpp
   AdditionalActionApplication.h
   application.cpp
   application.h
   AuthTracker.cpp
   AuthTracker.h
   BuildTime.cpp
   BuildTime.h
   #Clipboard2.cpp
   #Clipboard2.h
   ControlAppAuthenticator.cpp
   ControlAppAuthenticator.h
   ControlClient.cpp
   ControlClient.h
   ControlServer.cpp
   ControlServer.h
   #CrashHook.cpp
   #CrashHook.h
   DesktopServerApplication.cpp
   DesktopServerApplication.h
   DesktopServerCommandLine.cpp
   DesktopServerCommandLine.h
   ExtraRfbServers.cpp
   ExtraRfbServers.h
   framework.cpp
   framework.h
   LogInitListener.h
   #NamingDefs.cpp
   #NamingDefs.h
   #NewConnectionEvents.h
   OutgoingRfbConnectionThread.cpp
   OutgoingRfbConnectionThread.h
   QueryConnectionApplication.cpp
   QueryConnectionApplication.h
   QueryConnectionCommandLine.cpp
   QueryConnectionCommandLine.h
   QueryConnectionDialog.cpp
   QueryConnectionDialog.h
   #remoting_node.cpp
   resource.h
   RfbClientManager.cpp
   RfbClientManager.h
   RfbClientManagerEventListener.cpp
   RfbClientManagerEventListener.h
   RfbServer.cpp
   RfbServer.h
   Server.cpp
   Server.h
   ServerApplication.cpp
   ServerApplication.h
   ServerApplication_main_node.cpp
   ServerCommandLine.cpp
   ServerCommandLine.h
   ServiceControlApplication.cpp
   ServiceControlApplication.h
   ServiceControlCommandLine.cpp
   ServiceControlCommandLine.h
   ServerHelp.cpp
   ServerHelp.h
   ServerListener.cpp
   ServerListener.h
   Service.cpp
   Service.h
   EventLogWriter.cpp
   EventLogWriter.h
   ServiceEvents.h
   WsConfigRunner.cpp
   WsConfigRunner.h
)


list(APPEND library_references
   acme
   apex
   subsystem
   innate_subsystem
   remoting
   #remoting_control_desktop
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
add_dependencies(${PROJECT_NAME} remoting)
#add_dependencies(${PROJECT_NAME} remoting)
#add_dependencies(${PROJECT_NAME} remoting_client_config_lib)
#add_dependencies(${PROJECT_NAME} remoting_config_lib)
#add_dependencies(${PROJECT_NAME} remoting_file_lib)
#add_dependencies(${PROJECT_NAME} remoting_ftp_common)
#add_dependencies(${PROJECT_NAME} remoting_ftp_client)
#add_dependencies(${PROJECT_NAME} remoting_gui)
#add_dependencies(${PROJECT_NAME} remoting_io_lib)
#add_dependencies(${PROJECT_NAME} remoting_libjpeg)
#add_dependencies(${PROJECT_NAME} remoting_log_writer)
#add_dependencies(${PROJECT_NAME} remoting_zlib)
#add_dependencies(${PROJECT_NAME} remoting_network)
#add_dependencies(${PROJECT_NAME} remoting_region)
#add_dependencies(${PROJECT_NAME} remoting_rfb)
#add_dependencies(${PROJECT_NAME} remoting_thread)
#add_dependencies(${PROJECT_NAME} remoting_util)
#add_dependencies(${PROJECT_NAME} remoting_viewer_core)
#add_dependencies(${PROJECT_NAME} remoting_win_system)
#add_dependencies(${PROJECT_NAME} remoting_zlib)


list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND executable_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${executable_include_directories})

add_subdirectory(__implement)


target_compile_definitions(${PROJECT_NAME} PRIVATE _${PROJECT_NAME}_project)


