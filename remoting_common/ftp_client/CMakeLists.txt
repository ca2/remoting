cmake_minimum_required(VERSION 3.16)
project(remoting_ftp_client)


list(APPEND library_source
   framework.cpp
   framework.h
   UploadOperation.h
   RemoteFileRenameOperation.h
   resource.h
   RemoteFilesDeleteOperation.h
   OperationSupport.h
   RemoteFileListOperation.cpp
   OperationNotPermittedException.h
   RemoteFileListOperation.h
   RemoteFilesDeleteOperation.cpp
   OperationNotPermittedException.cpp
   OperationSupport.cpp
   OperationEventListener.h
   RemoteFolderCreateOperation.cpp
   RemoteFolderCreateOperation.h
   OperationEventListener.cpp
   RemoteFileRenameOperation.cpp
   UploadOperation.cpp
   LocalFilesDeleteOperation.h
   LocalFilesDeleteOperation.cpp
   FileTransferRequestSender.h
   FileTransferReplyBuffer.cpp
   FileTransferRequestSender.cpp
   FileTransferOperation.cpp
   FileTransferInterface.cpp
   FileTransferEventAdapter.cpp
   DownloadOperation.h
   DownloadOperation.cpp
   CopyOperation.h
   FileTransferOperation.h
   FileTransferEventHandler.cpp
   FileTransferEventHandler.h
   FileInfoList.h
   CopyOperation.cpp
   FileTransferMessageProcessor.h
   FileTransferCore.cpp
   FileInfoList.cpp
   CopyFileEventListener.h
   FileTransferReplyBuffer.h
   FileTransferEventAdapter.h
   FileTransferInterface.h
   FileTransferMessageProcessor.cpp
   FileTransferCore.h
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

