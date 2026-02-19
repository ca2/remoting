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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "FileTransferCore.h"

#include "RemoteFileListOperation.h"
#include "RemoteFilesDeleteOperation.h"
#include "RemoteFolderCreateOperation.h"
#include "RemoteFileRenameOperation.h"
#include "LocalFilesDeleteOperation.h"
#include "UploadOperation.h"
#include "DownloadOperation.h"

//#include "file_lib/::file::item.h"
#include "remoting/ftp_common/FolderListener.h"

namespace remoting
{
   namespace ftp
   {
      FileTransferCore::FileTransferCore(LogWriter *logWriter,
                                         FileTransferRequestSender *sender,
                                         FileTransferReplyBuffer *replyBuffer,
                                         ListenerContainer<FileTransferEventHandler *> *ftListeners)
      : m_logWriter(logWriter),
        m_state(NOTHING_STATE),
        m_sender(sender), m_replyBuffer(replyBuffer),
        m_fileTransferListeners(ftListeners),
        m_currentOperation(0)
      {
      }

      FileTransferCore::~FileTransferCore()
      {
         if (m_currentOperation != NULL) {
            delete m_currentOperation;
         }
      }

      void FileTransferCore::dataChunkCopied(unsigned long long totalBytesCopied, unsigned long long totalBytesToCopy)
      {
         if (m_state == NOTHING_STATE) {
            return;
         }

         if (totalBytesToCopy != 0) {
            double progress = 1.0 * totalBytesCopied / totalBytesToCopy;

            // event set pos to progress bar
            m_ftInterface->setProgress(progress);
         }
      }

      int FileTransferCore::targetFileExists(FileInfo *sourceFileInfo,
                                             FileInfo *targetFileInfo,
                                             const ::file::path & pathToTargetFile)
      {
         return m_ftInterface->onFtTargetFileExists(sourceFileInfo,
                                                    targetFileInfo,
                                                    pathToTargetFile);
      }

      void FileTransferCore::setInterface(FileTransferInterface *ftInterface)
      {
         m_ftInterface = ftInterface;
      }

      const OperationSupport &FileTransferCore::getSupportedOps()
      {
         return m_supportedOps;
      }


      ::pointer_array<FileInfo> FileTransferCore::getListLocalFolder(const ::file::path & pathToFile)
      {

         FolderListener fl(m_logWriter, pathToFile);

         if (!fl.list()) {
            throw "FileTransferCore: listLocalFolder";
         }
         //m_localFilesInfo.resize(fl.getFilesCount());
         //for (unsigned int i = 0; i < fl.getFilesCount(); i++)
         //   m_localFilesInfo[i] = fl.getFilesInfo()[i];

         m_localFilesInfo = fl.m_fileinfoa;

         return m_localFilesInfo;

      }


      ::pointer_array<FileInfo> FileTransferCore::getListRemoteFolder()
      {
         return m_remoteFilesInfo;
      }

      void FileTransferCore::updateSupportedOperations(const ::array_base<unsigned int> *clientCaps,
                                                       const ::array_base<unsigned int> *serverCaps)
      {
         m_supportedOps = OperationSupport(*clientCaps, *serverCaps);
      }

      void FileTransferCore::ftOpStarted(FileTransferOperation *sender)
      {
         m_ftInterface->onFtOpStarted();
      }

      void FileTransferCore::ftOpFinished(FileTransferOperation *sender)
      {
         //
         // If operation that sends to us this notification is remote file ::list_base operation
         // than we need to copy received file ::list_base to class members.
         //

         if (m_state == FILE_LIST_STATE) {
            RemoteFileListOperation *fileListOp = dynamic_cast<RemoteFileListOperation *>(sender);

            if (fileListOp->isOk()) {

               //
               // Copy files info to class members
               //

               // unsigned int numberFiles = m_replyBuffer->getFilesInfoCount();
               // m_remoteFilesInfo.resize(numberFiles);
               // for (unsigned int i = 0; i < numberFiles; i++) {
               //    m_remoteFilesInfo[i] = m_replyBuffer->getFilesInfo()[i];

                  m_remoteFilesInfo = m_replyBuffer->m_fileinfoa;
               //} // for all files in reply
            } // if no error during file ::list_base operation

            // Notify dialog than operation is finished
            int result = fileListOp->isOk() ? 1 : 0;
            m_ftInterface->onFtOpFinished(m_state, result);
            return ;
         } // if last executed operation was file ::list_base opearation

         //
         // Remark: we must post this notification scopedstrMessage to this dialog to avoid deadlock.
         // This deadlock reason described above:
         //
         // Some ft operations like upload, remote file delete, remote file rename operations
         // after operation finished must refresh remote file ::list_base, but we can do that only
         // with using remote RemoteFileListOperation.
         // Before RemoteFileListOperation can start execution, old operation must be removed
         // from ft scopedstrMessage processor listeners ::list_base, but it's locked by ft scopedstrMessage processor
         // and call of remove method also tries to lock operation.
         //
         // And as result: deadlock.
         //
         // In short:
         // ft scopedstrMessage processor lock operation
         // locked operation call this dialog as ft events listener with op finished notification
         // this dialog calls ft scopedstrMessage processor to remove operation from listeners ::list_base
         // when ft scopedstrMessage processor tries to remove operation it locks operation first.
         //
         // And as result: deadlock.
         //
         // FIXME: Create new scheme with such kind of file transfer operations.
         //

         m_ftInterface->onFtOpFinished(m_state, 0);
      } // void

      void FileTransferCore::ftOpErrorMessage(FileTransferOperation *sender,
                                              const ::scoped_string & scopedstrMessage)
      {
         m_ftInterface->onFtOpError(scopedstrMessage);
      }

      void FileTransferCore::ftOpInfoMessage(FileTransferOperation *sender,
                                             const ::scoped_string & scopedstrMessage)
      {
         m_ftInterface->onFtOpInfo(scopedstrMessage);
      }

      void FileTransferCore::executeOperation(FileTransferOperation *newOperation)

      {
         //
         // Cleanup, free previous operation
         //

         if (m_currentOperation != NULL) {
            m_currentOperation->removeListener(this);
            m_fileTransferListeners->removeListener(m_currentOperation);
            delete m_currentOperation;
         }

         m_currentOperation = newOperation;

         m_currentOperation->setRequestSender(m_sender);
         m_currentOperation->setReplyBuffer(m_replyBuffer);
         m_currentOperation->addListener(this);

         m_fileTransferListeners->addListener(m_currentOperation);

         try {
            m_currentOperation->do_file_transfer_operation();
         } catch (::io_exception &ioEx) {
            m_ftInterface->raise(ioEx);
         } // try / catch
      }

      void FileTransferCore::downloadOperation(const ::pointer_array< FileInfo > & fileinfoa,
                                               const ::file::path & pathToTargetRoot,
                                               const ::file::path & pathToSourceRoot)
      {
         m_state = DOWNLOAD_STATE;

         DownloadOperation *dOp = new DownloadOperation(m_logWriter,
                                                        fileinfoa,
                                                        pathToTargetRoot,
                                                        pathToSourceRoot);
         dOp->setCopyProcessListener(this);
         executeOperation(dOp);
      }

      void FileTransferCore::uploadOperation(const ::pointer_array<FileInfo> & fileinfoa,
                                             const ::file::path & pathToSourceRoot,
                                             const ::file::path & pathToTargetRoot)
      {
         m_state = UPLOAD_STATE;

         UploadOperation *uOp = new UploadOperation(m_logWriter,
                                                    fileinfoa,
                                                    pathToSourceRoot,
                                                    pathToTargetRoot);
         uOp->setCopyProcessListener(this);
         executeOperation(uOp);
      }

      void FileTransferCore::localFilesDeleteOperation(const ::pointer_array< FileInfo > & fileinfoa,
                                                       const ::file::path & pathToTargetRoot)
      {
         m_state = LOCAL_REMOVE_STATE;
         executeOperation(new LocalFilesDeleteOperation(m_logWriter,
                                                        fileinfoa,
                                                        pathToTargetRoot));
      }

      void FileTransferCore::remoteFilesDeleteOperation(const ::pointer_array< FileInfo > & fileinfoa,
                                                       const ::file::path & pathToTargetRoot)
      {
         m_state = REMOVE_STATE;
         executeOperation(new RemoteFilesDeleteOperation(m_logWriter,
                                                         fileinfoa,
                                                         pathToTargetRoot));
      }

      void FileTransferCore::remoteFolderCreateOperation(FileInfo file, const ::file::path & pathToTargetRoot)
      {
         m_state = MKDIR_STATE;

         executeOperation(new RemoteFolderCreateOperation(m_logWriter,
                                                          file,
                                                          pathToTargetRoot));
      }
      void FileTransferCore::remoteFileRenameOperation(FileInfo sourceFileInfo,
                                                       FileInfo targetFileInfo,
                                                       const ::file::path & pathToTargetRoot)
      {
         m_state = RENAME_STATE;

         executeOperation(new RemoteFileRenameOperation(m_logWriter,
                                                        sourceFileInfo,
                                                        targetFileInfo,
                                                        pathToTargetRoot));
      }

      void FileTransferCore::remoteFileListOperation(const ::file::path & pathToFile)
      {
         m_state = FILE_LIST_STATE;
         executeOperation(new RemoteFileListOperation(m_logWriter, pathToFile));
      }

      void FileTransferCore::terminateCurrentOperation()
      {
         m_currentOperation->terminate();
      }

      void FileTransferCore::onUpdateState(int state, int result)
      {
         switch (state) {
            case FILE_LIST_STATE:
               m_state = NOTHING_STATE;

               if (result == 0) {
                  break;
               }

               m_ftInterface->setNothingState();

               break;
            case REMOVE_STATE:
            case MKDIR_STATE:
            case RENAME_STATE:
            case UPLOAD_STATE:
               m_ftInterface->onRefreshRemoteFileList();
               break;

            case LOCAL_REMOVE_STATE:
            case DOWNLOAD_STATE:
               m_ftInterface->onRefreshLocalFileList();
               m_state = NOTHING_STATE;
               break;
         } // switch

      }

      bool FileTransferCore::isNothingState()
      {
         return m_state == NOTHING_STATE;
      }

      void FileTransferCore::onOperationFinished()
      {
         // Operation is finished, remove it from file transfer scopedstrMessage handlers
         m_fileTransferListeners->removeListener(m_currentOperation);
      }
   }
}