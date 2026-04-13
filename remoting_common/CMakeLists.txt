cmake_minimum_required(VERSION 3.16)
project(remoting_common)





list(APPEND library_source
   client_config/ConnectionConfig.cpp
   client_config/ConnectionConfig.h
   client_config/ConnectionConfigSM.cpp
   client_config/ConnectionConfigSM.h
   client_config/ViewerConfig.cpp
   client_config/ViewerConfig.h
   client_config/ViewerSettingsManager.cpp
   client_config/ViewerSettingsManager.h
   config/ConnectionHistory.cpp
   config/IniFileSettingsManager.cpp
   config/IniFileSettingsManager.h
   config/RegistrySettingsManager.cpp
   config/RegistrySettingsManager.h
   config/SettingsManager.cpp
   config/SettingsManager.h
   #ftp_client/_.cpp
   #ftp_client/_.h
   #ftp_client/_____debug.xcconfig
   #ftp_client/_____release.xcconfig
   #ftp_client/aaa_framework.cpp
   #ftp_client/aaa_framework.h
   #ftp_client/CMakeLists.txt
   ftp_client/CopyFileEventListener.h
   ftp_client/CopyOperation.cpp
   ftp_client/CopyOperation.h
   ftp_client/DownloadOperation.cpp
   ftp_client/DownloadOperation.h
   ftp_client/FileInfoList.cpp
   ftp_client/FileInfoList.h
   ftp_client/FileTransferCore.cpp
   ftp_client/FileTransferCore.h
   ftp_client/FileTransferEventAdapter.cpp
   ftp_client/FileTransferEventAdapter.h
   ftp_client/FileTransferEventHandler.cpp
   ftp_client/FileTransferEventHandler.h
   ftp_client/FileTransferInterface.cpp
   ftp_client/FileTransferInterface.h
   ftp_client/FileTransferMessageProcessor.cpp
   ftp_client/FileTransferMessageProcessor.h
   ftp_client/FileTransferOperation.cpp
   ftp_client/FileTransferOperation.h
   ftp_client/FileTransferReplyBuffer.cpp
   ftp_client/FileTransferReplyBuffer.h
   ftp_client/FileTransferRequestSender.cpp
   ftp_client/FileTransferRequestSender.h
   #ftp_client/ft_client_lib.vcproj
   ftp_client/LocalFilesDeleteOperation.cpp
   ftp_client/LocalFilesDeleteOperation.h
   #ftp_client/matter.txt
   ftp_client/OperationEventListener.cpp
   ftp_client/OperationEventListener.h
   ftp_client/OperationNotPermittedException.cpp
   ftp_client/OperationNotPermittedException.h
   ftp_client/OperationSupport.cpp
   ftp_client/OperationSupport.h
   ftp_client/RemoteFileListOperation.cpp
   ftp_client/RemoteFileListOperation.h
   ftp_client/RemoteFileRenameOperation.cpp
   ftp_client/RemoteFileRenameOperation.h
   ftp_client/RemoteFilesDeleteOperation.cpp
   ftp_client/RemoteFilesDeleteOperation.h
   ftp_client/RemoteFolderCreateOperation.cpp
   ftp_client/RemoteFolderCreateOperation.h
   #ftp_client/remoting_ftp_client.vcxproj
   #ftp_client/remoting_ftp_client.vcxproj.filters
   #ftp_client/remoting_ftp_client.vcxproj.user
   ftp_client/resource.h
   ftp_client/UploadOperation.cpp
   ftp_client/UploadOperation.h
   #ftp_common/_.cpp
   #ftp_common/_.h
   #ftp_common/_____debug.xcconfig
   #ftp_common/_____release.xcconfig
   #ftp_common/aaa_framework.cpp
   #ftp_common/aaa_framework.h
   #ftp_common/CMakeLists.txt
   ftp_common/FileInfo.cpp
   ftp_common/FileInfo.h
   ftp_common/FileTransferException.cpp
   ftp_common/FileTransferException.h
   ftp_common/FolderListener.cpp
   ftp_common/FolderListener.h
   #ftp_common/ft_common.vcproj
   ftp_common/FTMessage.cpp
   ftp_common/FTMessage.h
   #ftp_common/matter.txt
   ftp_common/OperationNotSupportedException.cpp
   ftp_common/OperationNotSupportedException.h
   #ftp_common/remoting_ftp_common.vcxproj
   #ftp_common/remoting_ftp_common.vcxproj.filters
   #ftp_common/remoting_ftp_common.vcxproj.user
   network/RfbInputGate.cpp
   network/RfbInputGate.h
   network/RfbOutputGate.cpp
   network/RfbOutputGate.h
   network/TcpClientThread.cpp
   network/TcpClientThread.h
   network/TcpServer.cpp
   network/TcpServer.h
   ftp_common/resource.h
   ftp_common/WinFilePath.cpp
   ftp_common/WinFilePath.h
   region/RectSerializer.cpp
   region/RectSerializer.h
   region/Region.cpp
   region/Region.h
   region/x11region.c
   rfb/AuthDefs.cpp
   rfb/AuthDefs.h
   rfb/CursorShape.cpp
   rfb/CursorShape.h
   rfb/EncodingDefs.cpp
   rfb/EncodingDefs.h
   rfb/HostPath.cpp
   rfb/HostPath.h
   rfb/MsgDefs.cpp
   rfb/MsgDefs.h
   rfb/PixelConverter.cpp
   rfb/PixelConverter.h
   rfb/RfbKeySym.cpp
   rfb/RfbKeySym.h
   rfb/RfbKeySymListener.h
   rfb/TunnelDefs.cpp
   rfb/TunnelDefs.h
   rfb/VendorDefs.cpp
   rfb/VendorDefs.h
   util/CommandLineFormatException.cpp
   util/CommandLineFormatException.h
   util/CommandLineFormatHelp.cpp
   util/CommandLineFormatHelp.h
   util/DesCrypt.cpp
   util/DesCrypt.h
   util/Keymap.cpp
   util/Keymap.h
   util/VncPassCrypt.cpp
   util/VncPassCrypt.h
   viewer_core/_.cpp
   viewer_core/_.h
   viewer_core/_____debug.xcconfig
   viewer_core/_____release.xcconfig
   viewer_core/aaa_framework.cpp
   viewer_core/aaa_framework.h
   viewer_core/AuthHandler.cpp
   viewer_core/AuthHandler.h
   viewer_core/CapabilitiesManager.cpp
   viewer_core/CapabilitiesManager.h
   viewer_core/CapsContainer.cpp
   viewer_core/CapsContainer.h
   viewer_core/CMakeLists.txt
   viewer_core/CompressionLevel.cpp
   viewer_core/CompressionLevel.h
   viewer_core/CopyRectDecoder.cpp
   viewer_core/CopyRectDecoder.h
   viewer_core/CoreEventsAdapter.cpp
   viewer_core/CoreEventsAdapter.h
   viewer_core/CursorPainter.cpp
   viewer_core/CursorPainter.h
   viewer_core/Decoder.cpp
   viewer_core/Decoder.h
   viewer_core/DecoderOfRectangle.cpp
   viewer_core/DecoderOfRectangle.h
   viewer_core/DecoderStore.cpp
   viewer_core/DecoderStore.h
   viewer_core/DesktopSizeDecoder.cpp
   viewer_core/DesktopSizeDecoder.h
   viewer_core/DispatchDataProvider.h
   viewer_core/drawTile.cpp
   viewer_core/FbUpdateNotifier.cpp
   viewer_core/FbUpdateNotifier.h
   viewer_core/FileTransferCapability.cpp
   viewer_core/FileTransferCapability.h
   viewer_core/HexTileDecoder.cpp
   viewer_core/HexTileDecoder.h
   viewer_core/JpegDecompressor.cpp
   viewer_core/JpegDecompressor.h
   viewer_core/JpegQualityLevel.cpp
   viewer_core/JpegQualityLevel.h
   viewer_core/LastRectDecoder.cpp
   viewer_core/LastRectDecoder.h
   viewer_core/matter.txt
   viewer_core/PointerPosDecoder.cpp
   viewer_core/PointerPosDecoder.h
   viewer_core/PseudoDecoder.cpp
   viewer_core/PseudoDecoder.h
   viewer_core/RawDecoder.cpp
   viewer_core/RawDecoder.h
   viewer_core/RemoteViewerCore.cpp
   viewer_core/RemoteViewerCore.h
   viewer_core/remoting_viewer_core.vcxproj
   viewer_core/remoting_viewer_core.vcxproj.filters
   viewer_core/remoting_viewer_core.vcxproj.user
   viewer_core/resource.h
   viewer_core/RfbClientToServerMessage.cpp
   viewer_core/RfbClientToServerMessage.h
   viewer_core/RfbCutTextEventClientMessage.cpp
   viewer_core/RfbCutTextEventClientMessage.h
   viewer_core/RfbFramebufferUpdateRequestClientMessage.cpp
   viewer_core/RfbFramebufferUpdateRequestClientMessage.h
   viewer_core/RfbKeyEventClientMessage.cpp
   viewer_core/RfbKeyEventClientMessage.h
   viewer_core/RfbPointerEventClientMessage.cpp
   viewer_core/RfbPointerEventClientMessage.h
   viewer_core/RfbSetEncodingsClientMessage.cpp
   viewer_core/RfbSetEncodingsClientMessage.h
   viewer_core/RfbSetPixelFormatClientMessage.cpp
   viewer_core/RfbSetPixelFormatClientMessage.h
   viewer_core/RichCursorDecoder.cpp
   viewer_core/RichCursorDecoder.h
   viewer_core/RreDecoder.cpp
   viewer_core/RreDecoder.h
   viewer_core/ServerMessageListener.h
   viewer_core/TcpConnection.cpp
   viewer_core/TcpConnection.h
   viewer_core/TightDecoder.cpp
   viewer_core/TightDecoder.h
   viewer_core/UpdateRequestSender.cpp
   viewer_core/UpdateRequestSender.h
   viewer_core/viewer_core.vcproj
   viewer_core/VncAuthentication.cpp
   viewer_core/VncAuthentication.h
   viewer_core/VncAuthenticationHandler.cpp
   viewer_core/VncAuthenticationHandler.h
   viewer_core/watermark_bmp.h
   viewer_core/WatermarksController.cpp
   viewer_core/WatermarksController.h
   viewer_core/ZrleDecoder.cpp
   viewer_core/ZrleDecoder.h
   _.cpp
   _.h
   EOFException.cpp
   EOFException.h
   framework.cpp
   framework.h
   remoting.cpp
   remoting.h
)


list(APPEND library_references
   acme
   subsystem
   apex
   innate_subsystem
)


list(APPEND library_include_directories
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(${PROJECT_NAME} SHARED ${library_source})

#target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)



if (NOT ${NO_PRECOMPILED_HEADER})

   target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
   #target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)
   set_source_files_properties(region/x11region.c PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif ()


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})


   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references} jpeg)


elseif (${LINUX} OR ${__BSD__} OR ${ANDROID})


   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)
   # target_link_options(static_${PROJECT_NAME} PRIVATE --whole-archive)

   message(STATUS "(5) PKGCONFIG_LIBRARIES is " ${${PROJECT_NAME}_PKGCONFIG_LIBRARIES})


endif ()


list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})



target_compile_definitions(${PROJECT_NAME} PRIVATE _${PROJECT_NAME}_project)



