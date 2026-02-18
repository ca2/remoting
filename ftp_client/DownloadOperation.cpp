// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "DownloadOperation.h"
#include "acme/filesystem/file/item.h"
#include "acme/filesystem/filesystem/file_context.h"


namespace remoting
{
   namespace ftp
   {
      DownloadOperation::DownloadOperation(LogWriter *logWriter,
                                           const ::pointer_array< FileInfo > & fileinfoa,
                                           const ::file::path & pathToTargetRoot,
                                           const ::file::path & pathToSourceRoot)
      : CopyOperation(logWriter),
        m_pfileitem(0),
        //m_pbinarystream(0),
        m_fileOffset(0),
        m_bufferSize(20000)
      {
         m_pathToSourceRoot = pathToSourceRoot;
         m_pathToTargetRoot = pathToTargetRoot;

         m_toCopy = new FileInfoList(logWriter, fileinfoa);

         changeFileToDownload(m_toCopy);
      }

      DownloadOperation::~DownloadOperation()
      {
         if (m_toCopy != NULL) {
            delete m_toCopy->getRoot();
         }

         m_pwriteable.release();

         m_pfileitem.release();
         // if (m_pbinarystream != NULL) {
         //    try { m_pbinarystream->m_pfile->close(); } catch (::io_exception) { }
         //    m_pbinarystream.release();
         // }
         // if (m_pfileitem != NULL) {
         //    delete m_pfileitem;
         // }
      }

      void DownloadOperation::do_file_transfer_operation()
      {
         m_foldersToCalcSizeLeft = 0;

         m_totalBytesToCopy = 0;
         m_totalBytesCopied = 0;

         // Notify listeners that operation have started
         notifyStart();

         //
         // Try to calculate input files size.
         //
         // Then this state will be finished, we can
         // start files download.
         //
         // See decFoldersToCalcSizeCount, onDirSizeReply, onLastRequestFailed
         // methods.
         //

         tryCalcInputFilesSize();

         if (m_foldersToCalcSizeLeft == 0) {
            startDownload();
         }
      }

      void DownloadOperation::onFileListReply(DataInputStream * pinput)
      {
         m_toCopy->setChild(m_logWriter,m_replyBuffer->m_fileinfoa);
         gotoNext();
      }

      void DownloadOperation::onDownloadReply(DataInputStream * pinput)
      {
         //
         // Cleanup
         //

         m_pwriteable.release();

         m_pfileitem.release();

         // if (m_pbinarystream != NULL) {
         //    try { m_pbinarystream->m_pfile->close(); } catch (...) { }
         //    delete m_pbinarystream;
         // }
         // if (m_pfileitem != NULL) {
         //    delete m_pfileitem;
         // }

         //
         // Try to create file on local file system and open it for writting
         //

         m_pfileitem = ::system()->file_item(m_pathToTargetFile);

         if (m_fileOffset == 0) {
            m_pfileitem->truncate();
         }

         try {
            //::string path;
            auto path = m_pfileitem->final_path();
            //øconstruct_new(m_pbinarystream);

            m_pwriteable = file()->get_writer(path);
            // Seek to initial file position to continue writting
            m_pwriteable->set_position((long long)m_fileOffset);
            m_totalBytesCopied += m_fileOffset;
         } catch (::remoting::Exception &ioEx) {
            notifyFailedToDownload(ioEx.get_message());
            gotoNext();
            return;
         }

         //
         // Send first request for file data
         //

         bool compression = m_replyBuffer->isCompressionSupported();

         m_sender->sendDownloadDataRequest((unsigned int)m_bufferSize,
                                           compression);
         m_lastRequestTime = ::earth::time::now();
      }

      void DownloadOperation::onDownloadDataReply(DataInputStream * pinput)
      {
         if (isTerminating()) {
            gotoNext();
            return ;
         }

         try {
            DataOutputStream dos(m_pwriteable);
            if (!m_replyBuffer->getDownloadBuffer().empty()) {
               dos.write(m_replyBuffer->getDownloadBuffer().data(),
                              m_replyBuffer->getDownloadBufferSize());
            }
         } catch (::io_exception &ioEx) {
            notifyFailedToDownload(ioEx.get_message());
            gotoNext();
            return ;
         }

         //
         // Notify that we receive some data
         //

         m_totalBytesCopied += m_replyBuffer->getDownloadBufferSize();

         if (m_copyListener != NULL) {
            m_copyListener->dataChunkCopied(m_totalBytesCopied, m_totalBytesToCopy);
         }

         //
         // Send next download data request
         //

         bool compression = m_replyBuffer->isCompressionSupported();

         if (m_lastRequestTime.elapsed() > 300_s) {
            m_bufferSize /= 2;
         } else {
            m_bufferSize *= 2;
         }
         if (m_bufferSize < 20000) {
            m_bufferSize = 20000;
         }
         if (m_bufferSize > 400000) {
            m_bufferSize = 400000;
         }
         m_sender->sendDownloadDataRequest((unsigned int)m_bufferSize, compression);
         m_lastRequestTime = ::earth::time::now();
      }

      void DownloadOperation::onDownloadEndReply(DataInputStream * pinput)
      {
         //
         // Cleanup
         //

         try { m_pwriteable->close(); } catch (...) { }
         //delete m_pbinarystream;
         //m_pbinarystream = NULL;
         m_pwriteable.release();

         try {
            m_pfileitem->set_last_modified_time(m_replyBuffer->getDownloadLastModified());
         } catch (...) {
            notifyFailedToDownload("Cannot set modification time");
         }

         delete m_pfileitem;
         m_pfileitem = NULL;

         gotoNext();
      }

      void DownloadOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         //
         // This LRF scopedstrMessage received from get folder size request
         // we do need to download next file
         //

         if (m_foldersToCalcSizeLeft > 0) {
            decFoldersToCalcSizeCount();
         } else {
            // Logging
           // ::string scopedstrMessage;

            auto strMessage = m_replyBuffer->getLastErrorMessage();

            notifyFailedToDownload(strMessage);

            // Download next file
            gotoNext();
         }
      }

      void DownloadOperation::onDirSizeReply(DataInputStream * pinput)
      {
         m_totalBytesToCopy += m_replyBuffer->getDirSize();
         decFoldersToCalcSizeCount();
      }

      void DownloadOperation::startDownload()
      {
         if (isTerminating()) {
            killOp();
            return ;
         } // if terminating

         FileInfo *fileInfo = m_toCopy->getFileInfo();

         // Logging
         if (m_toCopy->getFirst()->getParent() == NULL) {
            ::string scopedstrMessage;

            scopedstrMessage.format("Downloading '{}' {}", m_pathToTargetFile,
                           fileInfo->isDirectory() ? "folder" : "file");

            notifyInformation(scopedstrMessage);
         } // logging

         if (fileInfo->isDirectory()) {
            processFolder();
         } else {
            processFile();
         } // if not directory

         if (isTerminating()) {
            killOp();
            return ;
         } // if terminating
      }

      void DownloadOperation::processFile()
      {
         m_fileOffset = 0;

         auto targetFile = file_item(m_pathToTargetFile);

         if (targetFile->exists()) {
            FileInfo *sourceFileInfo = m_toCopy->getFileInfo();
            FileInfo targetFileInfo(targetFile);

            //
            // Copy listener must decide what to do with this situation
            //

            int action = m_copyListener->targetFileExists(sourceFileInfo,
                                                          &targetFileInfo,
                                                          m_pathToTargetFile);
            switch (action) {
               case CopyFileEventListener::TFE_OVERWRITE:
                  break;
               case CopyFileEventListener::TFE_SKIP:
                  m_totalBytesCopied += sourceFileInfo->getSize();
                  gotoNext();
                  return ;
               case CopyFileEventListener::TFE_APPEND:
                  m_fileOffset = targetFileInfo.getSize();
                  break;
               case CopyFileEventListener::TFE_CANCEL:
                  if (!isTerminating()) {
                     terminate();
                  } // if not terminating
                  return ;
               default:
                  _ASSERT(FALSE);
            } // switch
         } // if target file exists

         // Send request that we want to download file
         m_sender->sendDownloadRequest(m_pathToSourceFile, m_fileOffset);
      }

      void DownloadOperation::processFolder()
      {
         auto local = file_item(m_pathToTargetFile);
         if (local->exists() && local->is_folder()) {
         } else {
            try
            {
               local->create_folder();
            }
            catch (...)
            {
               // Logging
               ::string scopedstrMessage;

               scopedstrMessage.format("Error: failed to create local folder '{}'",
                              m_pathToTargetFile);

               notifyError(scopedstrMessage);

               // Download next file
               gotoNext();
               return ;
            }
         }

         m_sender->sendFileListRequest(m_pathToSourceFile,
                                       m_replyBuffer->isCompressionSupported());
      }

      void DownloadOperation::gotoNext()
      {
         FileInfoList *current = m_toCopy;

         bool hasChild = current->getChild() != NULL;
         bool hasNext = current->getNext() != NULL;
         bool hasParent = current->getFirst()->getParent() != NULL;

         if (hasChild) {
            // If it has child, we must download child file ::list_base first
            changeFileToDownload(current->getChild());
            startDownload();
         } else if (hasNext) {
            // If it has no child, but has next file, we must download next file
            changeFileToDownload(current->getNext());
            startDownload();
         } else {

            //
            // If it has no child and not next, but has parent file,
            // we must go to parent file (folder i mean), set childs to NULL
            // cause we already download child files and go to next file.
            //

            FileInfoList *first = current->getFirst();
            if (hasParent) {
               changeFileToDownload(first->getParent());
               m_toCopy->setChild(nullptr, {});

               gotoNext();
            } else {
               killOp();
            } // if / else
         } // if / else
      } // void

      void DownloadOperation::tryCalcInputFilesSize()
      {
         FileInfoList *fil = m_toCopy;

         while (fil != NULL) {
            if (fil->getFileInfo()->isDirectory()) {

               auto pathNoRoot = fil->getAbsolutePath();

               auto pathToFile= m_pathToSourceRoot;

               pathToFile /= pathNoRoot;

               m_foldersToCalcSizeLeft++;
               m_sender->sendFolderSizeRequest(pathToFile);
            } else {
               m_totalBytesToCopy += fil->getFileInfo()->getSize();
            }
            fil = fil->getNext();
         }
      }

      void DownloadOperation::killOp()
      {
         //
         // If not files to download, than clear memory and
         // operation is finished
         //

         delete m_toCopy->getRoot();
         m_toCopy = NULL;

         notifyFinish();
      }

      void DownloadOperation::decFoldersToCalcSizeCount()
      {
         m_foldersToCalcSizeLeft--;

         // No more folders to calc size, start download
         if (m_foldersToCalcSizeLeft == 0) {
            startDownload();
         }
      }

      void DownloadOperation::notifyFailedToDownload(const ::scoped_string & scopedstrErrorDescription)
      {
         ::string strMessage;

         strMessage.format("Error: failed to download '{}' ({})",
                        m_pathToSourceFile,
                        scopedstrErrorDescription);

         notifyError(strMessage);
      }

      void DownloadOperation::changeFileToDownload(FileInfoList *toDownload)
      {
         m_toCopy = toDownload;

         m_pathToSourceFile = getRemotePath(m_toCopy, m_pathToSourceRoot);
         m_pathToTargetFile = getLocalPath(m_toCopy, m_pathToTargetRoot);
      }
   }
}
