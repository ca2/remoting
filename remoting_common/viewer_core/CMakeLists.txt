cmake_minimum_required(VERSION 3.16)
project(remoting_viewer_core)


list(APPEND library_source
   framework.cpp
   framework.h
   AuthHandler.cpp
   CapabilitiesManager.cpp
   CapabilitiesManager.h
   CapsContainer.cpp
   CapsContainer.h
   AuthHandler.h
   CompressionLevel.cpp
   CursorPainter.h
   Decoder.cpp
   DesktopSizeDecoder.h
   CopyRectDecoder.cpp
   DecoderStore.cpp
   CoreEventsAdapter.h
   FbUpdateNotifier.h
   FileTransferCapability.h
   CursorPainter.cpp
   CoreEventsAdapter.cpp
   DesktopSizeDecoder.cpp
   Decoder.h
   DecoderStore.h
   DecoderOfRectangle.h
   CompressionLevel.h
   FbUpdateNotifier.cpp
   DecoderOfRectangle.cpp
   DispatchDataProvider.h
   CopyRectDecoder.h
   FileTransferCapability.cpp
   HexTileDecoder.cpp
   HexTileDecoder.h
   JpegQualityLevel.h
   LastRectDecoder.h
   JpegDecompressor.h
   JpegQualityLevel.cpp
   LastRectDecoder.cpp
   JpegDecompressor.cpp
   PointerPosDecoder.cpp
   PseudoDecoder.cpp
   RemoteViewerCore.h
   RawDecoder.h
   RemoteViewerCore.cpp
   RawDecoder.cpp
   PointerPosDecoder.h
   PseudoDecoder.h
   RfbClientToServerMessage.cpp
   RfbCutTextEventClientMessage.h
   RfbPointerEventClientMessage.h
   RichCursorDecoder.h
   RfbPointerEventClientMessage.cpp
   RfbSetEncodingsClientMessage.cpp
   RfbSetPixelFormatClientMessage.h
   RfbKeyEventClientMessage.cpp
   RfbSetEncodingsClientMessage.h
   RfbKeyEventClientMessage.h
   RfbFramebufferUpdateRequestClientMessage.h
   RfbFramebufferUpdateRequestClientMessage.cpp
   RreDecoder.cpp
   ServerMessageListener.h
   RichCursorDecoder.cpp
   RfbCutTextEventClientMessage.cpp
   RfbClientToServerMessage.h
   RfbSetPixelFormatClientMessage.cpp
   RreDecoder.h
   TcpConnection.cpp
   TightDecoder.h
   TcpConnection.h
   UpdateRequestSender.cpp
   TightDecoder.cpp
   UpdateRequestSender.h
   VncAuthentication.cpp
   VncAuthentication.h
   VncAuthenticationHandler.cpp
   WatermarksController.cpp
   VncAuthenticationHandler.h
   watermark_bmp.h
   ZrleDecoder.h
   WatermarksController.h
   ZrleDecoder.cpp
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






#target_link_directories(${PROJECT_NAME} PRIVATE ${library_link_directories})

target_precompile_headers(${PROJECT_NAME} PRIVATE framework.h)
#target_precompile_headers(static_${PROJECT_NAME} PRIVATE framework.h)

#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


if(${WIN32})
   target_compile_options(${PROJECT_NAME} PRIVATE "/MD$<$<CONFIG:Debug>:d>")
   target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
   target_link_options(${PROJECT_NAME} PRIVATE "/DEFAULTLIB:msvcrt$<$<CONFIG:Debug>:d>")
   target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})

elseif(${LINUX} OR ${__BSD__} OR ${ANDROID})
   target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive  ${EXTRA_CXX_TARGET_COMPILER_OPTIONS}>)
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-rpath=$ORIGIN)
   #target_link_libraries(${PROJECT_NAME} PRIVATE ${library_references} ${global_library_references})
   #target_link_options(${PROJECT_NAME} PRIVATE -Wl,-z,defs)

endif ()

list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/..)
list(APPEND library_include_directories ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_include_directories(${PROJECT_NAME} PRIVATE ${library_include_directories})

