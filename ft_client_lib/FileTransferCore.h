// Copyright (C) 2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "ft_common/FileInfo.h"
#include "log_writer/LogWriter.h"

#include "FileTransferEventHandler.h"
#include "FileTransferRequestSender.h"
#include "FileTransferReplyBuffer.h"
#include "FileTransferOperation.h"
#include "OperationEventListener.h"
#include "CopyFileEventListener.h"
#include "OperationSupport.h"

#include "FileTransferInterface.h"

class FileTransferInterface;

class FileTransferCore : public OperationEventListener,
                         public CopyFileEventListener
{
public:
  //
  // Creates new instance of class.
  //
  // Parameters:
  // sender - object that needs for file transfer operations
  // replyBuffer - object that needs for file transfer operations
  // ftListeners - needs for executing file transfer operations
  //

  FileTransferCore(LogWriter *logWriter,
                   FileTransferRequestSender *sender,
                   FileTransferReplyBuffer *replyBuffer,
                   ListenerContainer<FileTransferEventHandler *> *ftListeners);
  virtual ~FileTransferCore();

  void setInterface(FileTransferInterface *ftInterface);

  const OperationSupport &getSupportedOps();

  ::array_base<FileInfo> *getListLocalFolder(const ::scoped_string & scopedstrPathToFile);
  ::array_base<FileInfo> *getListRemoteFolder();

  void downloadOperation(const FileInfo *filesToDownload,
                         size_t filesCount,
                         const ::scoped_string & scopedstrPathToTargetRoot,
                         const ::scoped_string & scopedstrPathToSourceRoot);
  void uploadOperation(const FileInfo *filesToDownload,
                       size_t filesCount,
                       const ::scoped_string & scopedstrPathToSourceRoot,
                       const ::scoped_string & scopedstrPathToTargetRoot);
  void localFilesDeleteOperation(const FileInfo *filesToDelete,
                                 unsigned int filesCount,
                                 const ::scoped_string & scopedstrPathToTargetRoot);
  void remoteFilesDeleteOperation(const FileInfo *filesInfoToDelete,
                                  size_t filesCount,
                                  const ::scoped_string & scopedstrPathToTargetRoot);
  void remoteFolderCreateOperation(FileInfo file, const ::scoped_string & scopedstrPathToTargetRoot);
  void remoteFileRenameOperation(FileInfo sourceFileInfo,
                                 FileInfo targetFileInfo,
                                 const ::scoped_string & scopedstrPathToTargetRoot);
  void remoteFileListOperation(const ::scoped_string & scopedstrPathToFile);

  void terminateCurrentOperation();

  bool isNothingState();

  void onOperationFinished();
  void onUpdateState(int state, int result);

  // FIXME: Debug.
  void updateSupportedOperations(const ::array_base<unsigned int> *clientCaps,
                                 const ::array_base<unsigned int> *serverCaps);

protected:
  //
  // Inherited from CopyFileEventListener
  //

  virtual void dataChunkCopied(unsigned long long totalBytesCopied, unsigned long long totalBytesToCopy);

  virtual int targetFileExists(FileInfo *sourceFileInfo,
                               FileInfo *targetFileInfo,
                               const ::scoped_string & scopedstrPathToTargetFile);

  //
  // Inherited from OperationEventListener
  //

  virtual void ftOpStarted(FileTransferOperation *sender);
  virtual void ftOpFinished(FileTransferOperation *sender);
  virtual void ftOpErrorMessage(FileTransferOperation *sender, const ::scoped_string & scopedstrMessage);
  virtual void ftOpInfoMessage(FileTransferOperation *sender,
                               const ::scoped_string & scopedstrMessage);

  //
  // Interface of log
  //

  LogWriter *m_logWriter;

  //
  // Current dialog state, needed for checking what type of
  // file transfer operation is executing now (means nothing or
  // delete, upload, download etc).
  //

  int m_state;

  //
  // Current file transfer operation.
  // Remark: only one file operation can be executed at one time
  //

  FileTransferOperation *m_currentOperation;
  //
  // File transfer message listener container.
  //
  // This member needs for executing file transfer operations,
  // after operation is created, but before it has started it needs
  // to be added to these listeners, after it has finished work needs
  // to remove operation from listeners ::list.
  //

  ListenerContainer<FileTransferEventHandler *> *m_fileTransferListeners;
  //
  // Request sender, reply buffer
  //

  FileTransferRequestSender *m_sender;
  FileTransferReplyBuffer *m_replyBuffer;

  //
  // Class that knows about supported operations.
  //

  OperationSupport m_supportedOps;

  //
  // File ::list request variables
  //

  ::array_base <FileInfo> m_remoteFilesInfo;

  //
  // Local file ::list variables
  //

  ::array_base <FileInfo> m_localFilesInfo;

  //
  // file transfer interface
  //
  FileTransferInterface *m_ftInterface;

  //
  // Frees previous file transfer operation if it was, sets
  // common file transfer operation parameters to new operation, made
  // new operation current and executes it.
  //

  void executeOperation(FileTransferOperation *newOperation);
  //
  // Avaliable m_state values
  // FIXME: Maybe make m_state enumeration?
  //

  static const int NOTHING_STATE      = 0x0;
  static const int FILE_LIST_STATE    = 0x1;
  static const int REMOVE_STATE       = 0x2;
  static const int MKDIR_STATE        = 0x3;
  static const int RENAME_STATE       = 0x4;

  static const int LOCAL_REMOVE_STATE = 0x5;

  static const int UPLOAD_STATE       = 0x6;
  static const int DOWNLOAD_STATE     = 0x7;

};


