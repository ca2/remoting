cmake_minimum_required(VERSION 3.16)
project(remoting)





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
   desktop/_.h
   desktop/_____debug.xcconfig
   desktop/_____release.xcconfig
   desktop/aaa_framework.cpp
   desktop/aaa_framework.h
   desktop/aaa_resource.h
   desktop/aaaa_.cpp
   desktop/AbnormDeskTermListener.h
   desktop/ApplicationDesktopFactory.cpp
   desktop/ApplicationDesktopFactory.h
   desktop/CMakeLists.txt
   desktop/ConsolePoller.cpp
   desktop/ConsolePoller.h
   desktop/CopyRectDetector.cpp
   desktop/CopyRectDetector.h
   desktop/CursorPositionDetector.cpp
   desktop/CursorPositionDetector.h
   desktop/CursorShapeDetector.cpp
   desktop/CursorShapeDetector.h
   desktop/CursorShapeGrabber.cpp
   desktop/CursorShapeGrabber.h
   desktop/Desktop.h
   desktop/desktop.vcproj
   desktop/DesktopBaseImpl.cpp
   desktop/DesktopBaseImpl.h
   desktop/DesktopClientImpl.cpp
   desktop/DesktopClientImpl.h
   desktop/DesktopConfigLocal.cpp
   desktop/DesktopConfigLocal.h
   desktop/DesktopFactory.h
   desktop/DesktopServerWatcher.cpp
   desktop/DesktopServerWatcher.h
   desktop/DesktopWinImpl.cpp
   desktop/DesktopWinImpl.h
   desktop/DisplayEsc.h
   desktop/DummyScreenDriver.cpp
   desktop/DummyScreenDriver.h
   desktop/GrabOptimizator.cpp
   desktop/GrabOptimizator.h
   desktop/HookInstaller.cpp
   desktop/HookInstaller.h
   desktop/HooksUpdateDetector.cpp
   desktop/HooksUpdateDetector.h
   desktop/HookUpdateTimer.cpp
   desktop/HookUpdateTimer.h
   desktop/InputBlocker.cpp
   desktop/InputBlocker.h
   #desktop/matter.txt
   desktop/Poller.cpp
   desktop/Poller.h
   desktop/remoting_desktop.vcxproj
   desktop/remoting_desktop.vcxproj.filters
   desktop/remoting_desktop.vcxproj.user
   desktop/SasUserInput.cpp
   desktop/SasUserInput.h
   desktop/ScreenDriver.h
   desktop/ScreenDriverFactory.h
   desktop/ScreenGrabber.cpp
   desktop/ScreenGrabber.h
   desktop/UpdateContainer.cpp
   desktop/UpdateContainer.h
   desktop/UpdateDetector.cpp
   desktop/UpdateDetector.h
   desktop/UpdateFilter.cpp
   desktop/UpdateFilter.h
   desktop/UpdateHandler.cpp
   desktop/UpdateHandler.h
   desktop/UpdateHandlerImpl.cpp
   desktop/UpdateHandlerImpl.h
   desktop/UpdateKeeper.cpp
   desktop/UpdateKeeper.h
   desktop/UpdateListener.cpp
   desktop/UpdateListener.h
   desktop/UpdateSendingListener.cpp
   desktop/UpdateSendingListener.h
   desktop/UserInput.cpp
   desktop/UserInput.h
   desktop/WallpaperUtil.cpp
   desktop/WallpaperUtil.h
   desktop_ipc/_.h
   #desktop_ipc/_____debug.xcconfig
   #desktop_ipc/_____release.xcconfig
   #desktop_ipc/aaa__.cpp
   #desktop_ipc/aaa_framework.cpp
   #desktop_ipc/aaa_framework.h
   #desktop_ipc/aaa_resource.h
   desktop_ipc/BlockingGate.cpp
   desktop_ipc/BlockingGate.h
   desktop_ipc/BlockingInputGate.cpp
   desktop_ipc/BlockingInputGate.h
   desktop_ipc/BlockingOutputGate.cpp
   desktop_ipc/BlockingOutputGate.h
   desktop_ipc/ClientListener.cpp
   desktop_ipc/ClientListener.h
   #desktop_ipc/CMakeLists.txt
   desktop_ipc/ConfigServer.cpp
   desktop_ipc/ConfigServer.h
   #desktop_ipc/desktop_ipc.vcproj
   desktop_ipc/DesktopConfigClient.cpp
   desktop_ipc/DesktopConfigClient.h
   desktop_ipc/DesktopServerProto.cpp
   desktop_ipc/DesktopServerProto.h
   desktop_ipc/DesktopSrvDispatcher.cpp
   desktop_ipc/DesktopSrvDispatcher.h
   desktop_ipc/GateKicker.cpp
   desktop_ipc/GateKicker.h
   desktop_ipc/GateKickHandler.cpp
   desktop_ipc/GateKickHandler.h
   #desktop_ipc/matter.txt
   desktop_ipc/ReconnectException.cpp
   desktop_ipc/ReconnectException.h
   desktop_ipc/ReconnectingChannel.cpp
   desktop_ipc/ReconnectingChannel.h
   desktop_ipc/ReconnectionListener.h
   #desktop_ipc/remoting_desktop_ipc.vcxproj
   #desktop_ipc/remoting_desktop_ipc.vcxproj.filters
   #desktop_ipc/remoting_desktop_ipc.vcxproj.user
   desktop_ipc/UpdateHandlerClient.cpp
   desktop_ipc/UpdateHandlerClient.h
   desktop_ipc/UpdateHandlerServer.cpp
   desktop_ipc/UpdateHandlerServer.h
   desktop_ipc/UserInputClient.cpp
   desktop_ipc/UserInputClient.h
   desktop_ipc/UserInputServer.cpp
   desktop_ipc/UserInputServer.h
   framebuffer_update_sender/_.h
   #fb_update_sender/_____debug.xcconfig
   #fb_update_sender/_____release.xcconfig
   #fb_update_sender/aaa_.cpp
   #fb_update_sender/aaa_framework.cpp
   #fb_update_sender/aaa_framework.h
   #fb_update_sender/aaa_resource.h
   #fb_update_sender/CMakeLists.txt
   framebuffer_update_sender/CursorUpdates.cpp
   framebuffer_update_sender/CursorUpdates.h
   #fb_update_sender/fb_update_sender.vcproj
   #fb_update_sender/matter.txt
   #fb_update_sender/remoting_fb_update_sender.vcxproj
   #fb_update_sender/remoting_fb_update_sender.vcxproj.filters
   #fb_update_sender/remoting_fb_update_sender.vcxproj.user
   framebuffer_update_sender/SenderControlInformationInterface.h
   framebuffer_update_sender/UpdateRequestListener.h
   framebuffer_update_sender/UpdateSender.cpp
   framebuffer_update_sender/UpdateSender.h
   framebuffer_update_sender/UpdSenderMsgDefs.cpp
   framebuffer_update_sender/UpdSenderMsgDefs.h
   framebuffer_update_sender/Viewport.cpp
   framebuffer_update_sender/Viewport.h
   framebuffer_update_sender/ViewPortState.cpp
   framebuffer_update_sender/ViewPortState.h
   #ftp_client/_.cpp
   #ftp_client/_.h
   #ftp_client/_____debug.xcconfig
   #ftp_client/_____release.xcconfig
   #ftp_client/aaa_framework.cpp
   #ftp_client/aaa_framework.h
   #ftp_client/CMakeLists.txt
   file_transfer_client/CopyFileEventListener.h
   file_transfer_client/CopyOperation.cpp
   file_transfer_client/CopyOperation.h
   file_transfer_client/DownloadOperation.cpp
   file_transfer_client/DownloadOperation.h
   file_transfer_client/FileInfoList.cpp
   file_transfer_client/FileInfoList.h
   file_transfer_client/FileTransferCore.cpp
   file_transfer_client/FileTransferCore.h
   file_transfer_client/FileTransferEventAdapter.cpp
   file_transfer_client/FileTransferEventAdapter.h
   file_transfer_client/FileTransferEventHandler.cpp
   file_transfer_client/FileTransferEventHandler.h
   file_transfer_client/FileTransferInterface.cpp
   file_transfer_client/FileTransferInterface.h
   file_transfer_client/FileTransferMessageProcessor.cpp
   file_transfer_client/FileTransferMessageProcessor.h
   file_transfer_client/FileTransferOperation.cpp
   file_transfer_client/FileTransferOperation.h
   file_transfer_client/FileTransferReplyBuffer.cpp
   file_transfer_client/FileTransferReplyBuffer.h
   file_transfer_client/FileTransferRequestSender.cpp
   file_transfer_client/FileTransferRequestSender.h
   #ftp_client/ft_client_lib.vcproj
   file_transfer_client/LocalFilesDeleteOperation.cpp
   file_transfer_client/LocalFilesDeleteOperation.h
   #ftp_client/matter.txt
   file_transfer_client/OperationEventListener.cpp
   file_transfer_client/OperationEventListener.h
   file_transfer_client/OperationNotPermittedException.cpp
   file_transfer_client/OperationNotPermittedException.h
   file_transfer_client/OperationSupport.cpp
   file_transfer_client/OperationSupport.h
   file_transfer_client/RemoteFileListOperation.cpp
   file_transfer_client/RemoteFileListOperation.h
   file_transfer_client/RemoteFileRenameOperation.cpp
   file_transfer_client/RemoteFileRenameOperation.h
   file_transfer_client/RemoteFilesDeleteOperation.cpp
   file_transfer_client/RemoteFilesDeleteOperation.h
   file_transfer_client/RemoteFolderCreateOperation.cpp
   file_transfer_client/RemoteFolderCreateOperation.h
   #ftp_client/remoting_ftp_client.vcxproj
   #ftp_client/remoting_ftp_client.vcxproj.filters
   #ftp_client/remoting_ftp_client.vcxproj.user
   file_transfer_client/resource.h
   file_transfer_client/UploadOperation.cpp
   file_transfer_client/UploadOperation.h
   #ftp_common/_.cpp
   #ftp_common/_.h
   #ftp_common/_____debug.xcconfig
   #ftp_common/_____release.xcconfig
   #ftp_common/aaa_framework.cpp
   #ftp_common/aaa_framework.h
   #ftp_common/CMakeLists.txt
   file_transfer_common/FileInfo.cpp
   file_transfer_common/FileInfo.h
   file_transfer_common/FileTransferException.cpp
   file_transfer_common/FileTransferException.h
   file_transfer_common/FolderListener.cpp
   file_transfer_common/FolderListener.h
   #ftp_common/ft_common.vcproj
   file_transfer_common/FTMessage.cpp
   file_transfer_common/FTMessage.h
   #ftp_common/matter.txt
   file_transfer_common/OperationNotSupportedException.cpp
   file_transfer_common/OperationNotSupportedException.h
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
   file_transfer_common/resource.h
   file_transfer_common/WinFilePath.cpp
   file_transfer_common/WinFilePath.h
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
   rfb_sconn/_.h
   #rfb_sconn/_____debug.xcconfig
   #rfb_sconn/_____release.xcconfig
   #rfb_sconn/aaa_.cpp
   #rfb_sconn/aaa_framework.cpp
   #rfb_sconn/aaa_framework.h
   #rfb_sconn/aaa_resource.h
   rfb_sconn/AuthException.h
   rfb_sconn/CapContainer.cpp
   rfb_sconn/CapContainer.h
   rfb_sconn/ClientAuthListener.h
   rfb_sconn/ClientInputEventListener.h
   rfb_sconn/ClientInputHandler.cpp
   rfb_sconn/ClientInputHandler.h
   rfb_sconn/ClientTerminationListener.h
   rfb_sconn/ClipboardExchange.cpp
   rfb_sconn/ClipboardExchange.h
   #rfb_sconn/CMakeLists.txt
   rfb_sconn/EchoExtensionRequestHandler.cpp
   rfb_sconn/EchoExtensionRequestHandler.h
   rfb_sconn/EncodeOptions.cpp
   rfb_sconn/EncodeOptions.h
   rfb_sconn/Encoder.cpp
   rfb_sconn/Encoder.h
   rfb_sconn/EncoderStore.cpp
   rfb_sconn/EncoderStore.h
   rfb_sconn/HextileEncoder.cpp
   rfb_sconn/HextileEncoder.h
   rfb_sconn/HextileTile.h
   rfb_sconn/JpegCompressor.cpp
   rfb_sconn/JpegCompressor.h
   rfb_sconn/JpegEncoder.cpp
   rfb_sconn/JpegEncoder.h
   #rfb_sconn/matter.txt
   #rfb_sconn/remoting_rfb_sconn.vcxproj
   #rfb_sconn/remoting_rfb_sconn.vcxproj.filters
   #rfb_sconn/remoting_rfb_sconn.vcxproj.user
   #rfb_sconn/rfb_sconn.vcproj
   rfb_sconn/RfbClient.cpp
   rfb_sconn/RfbClient.h
   rfb_sconn/RfbCodeRegistrator.cpp
   rfb_sconn/RfbCodeRegistrator.h
   rfb_sconn/RfbDispatcher.cpp
   rfb_sconn/RfbDispatcher.h
   rfb_sconn/RfbDispatcherListener.h
   rfb_sconn/RfbInitializer.cpp
   rfb_sconn/RfbInitializer.h
   rfb_sconn/RreEncoder.cpp
   rfb_sconn/RreEncoder.h
   rfb_sconn/TightEncoder.cpp
   rfb_sconn/TightEncoder.h
   rfb_sconn/TightPalette.cpp
   rfb_sconn/TightPalette.h
   rfb_sconn/ZrleEncoder.cpp
   rfb_sconn/ZrleEncoder.h
   server/NewConnectionEvents.h
#   util/CommandLineFormatException.cpp
#   util/CommandLineFormatException.h
#   util/CommandLineFormatHelp.cpp
#   util/CommandLineFormatHelp.h
#   util/DesCrypt.cpp
#   util/DesCrypt.h
#   util/Keymap.cpp
#   util/Keymap.h
#   util/VncPassCrypt.cpp
#   util/VncPassCrypt.h
   server_config/_.h
   #server_config/_____debug.xcconfig
   #server_config/_____release.xcconfig
   #server_config/aaa_.cpp
   #server_config/aaa_framework.cpp
   #server_config/aaa_framework.h
   #server_config/CMakeLists.txt
   server_config/ConfigReloadListener.cpp
   server_config/ConfigReloadListener.h
   server_config/Configurator.cpp
   server_config/Configurator.h
   server_config/IpAccessControl.cpp
   server_config/IpAccessControl.h
   server_config/IpAccessRule.cpp
   server_config/IpAccessRule.h
   #server_config/matter.txt
   server_config/PortMapping.cpp
   server_config/PortMapping.h
   server_config/PortMappingContainer.cpp
   server_config/PortMappingContainer.h
   server_config/PortMappingRect.cpp
   server_config/PortMappingRect.h
   server_config/RegistrySecurityAttributes.cpp
   server_config/RegistrySecurityAttributes.h
   #server_config/remoting_server_config.vcxproj
   #server_config/remoting_server_config.vcxproj.filters
   #server_config/remoting_server_config.vcxproj.user
   #server_config/resource.h
   #server_config/server_config_lib.vcproj
   server_config/ServerConfig.cpp
   server_config/ServerConfig.h
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


if(${WIN32})

   list(APPEND library_source
   desktop/windows/MirrorDriverClient.cpp
   desktop/windows/MirrorDriverClient.h
   desktop/windows/MirrorScreenDriver.cpp
   desktop/windows/MirrorScreenDriver.h
   desktop/windows/Win8CursorShape.cpp
   desktop/windows/Win8CursorShape.h
   desktop/windows/Win8DeskDuplicationThread.cpp
   desktop/windows/Win8DeskDuplicationThread.h
   desktop/windows/Win8DuplicationListener.h
   desktop/windows/Win8ScreenDriver.cpp
   desktop/windows/Win8ScreenDriver.h
   desktop/windows/Win8ScreenDriverImpl.cpp
   desktop/windows/Win8ScreenDriverImpl.h
   desktop/windows/Win32MirrorScreenDriver.cpp
   desktop/windows/Win32MirrorScreenDriver.h
   desktop/windows/Win32ScreenDriver.cpp
   desktop/windows/Win32ScreenDriver.h
   desktop/windows/Win32ScreenDriverBaseImpl.cpp
   desktop/windows/Win32ScreenDriverBaseImpl.h
   desktop/windows/Win32ScreenDriverFactory.cpp
   desktop/windows/Win32ScreenDriverFactory.h
   desktop/windows/WinAutoMapDxgiSurface.cpp
   desktop/windows/WinAutoMapDxgiSurface.h
   desktop/windows/WinCursorShapeUtils.cpp
   desktop/windows/WinCursorShapeUtils.h
   desktop/windows/WinCustomD3D11Texture2D.cpp
   desktop/windows/WinCustomD3D11Texture2D.h
   desktop/windows/WinD3D11Device.cpp
   desktop/windows/WinD3D11Device.h
   desktop/windows/WinD3D11Texture2D.cpp
   desktop/windows/WinD3D11Texture2D.h
   desktop/windows/WindowsCursorShapeGrabber.cpp
   desktop/windows/WindowsCursorShapeGrabber.h
   desktop/windows/WindowsInputBlocker.cpp
   desktop/windows/WindowsInputBlocker.h
   desktop/windows/WindowsScreenGrabber.cpp
   desktop/windows/WindowsScreenGrabber.h
   desktop/windows/WindowsUserInput.cpp
   desktop/windows/WindowsUserInput.h
   desktop/windows/WinDxCriticalException.cpp
   desktop/windows/WinDxCriticalException.h
   desktop/windows/WinDxException.cpp
   desktop/windows/WinDxException.h
   desktop/windows/WinDxgiAcquiredFrame.cpp
   desktop/windows/WinDxgiAcquiredFrame.h
   desktop/windows/WinDxgiAdapter.cpp
   desktop/windows/WinDxgiAdapter.h
   desktop/windows/WinDxgiDevice.cpp
   desktop/windows/WinDxgiDevice.h
   desktop/windows/WinDxgiOutput.cpp
   desktop/windows/WinDxgiOutput.h
   desktop/windows/WinDxgiOutput1.cpp
   desktop/windows/WinDxgiOutput1.h
   desktop/windows/WinDxgiOutputDuplication.cpp
   desktop/windows/WinDxgiOutputDuplication.h
   desktop/windows/WinDxgiSurface.cpp
   desktop/windows/WinDxgiSurface.h
   desktop/windows/WinDxRecoverableException.cpp
   desktop/windows/WinDxRecoverableException.h
   desktop/windows/WinServiceDesktopFactory.cpp
   desktop/windows/WinServiceDesktopFactory.h
   desktop/windows/WinVideoRegionUpdaterImpl.cpp
   desktop/windows/WinVideoRegionUpdaterImpl.h

   )
endif()

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


add_library(${PROJECT_NAME} SHARED ${library_source}
   desktop/windows/_common_header.h)

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
   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references} port_jpeg)


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



