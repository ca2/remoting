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
#include "UploadOperation.h"
#include "acme/filesystem/file/item.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "acme/filesystem/filesystem/listing.h"
#include "remoting/ftp_common/WinFilePath.h"
#include "remoting/ftp_common/FolderListener.h"
#include "remoting/common/EOFException.h"

namespace remoting
{
   namespace ftp
   {
      UploadOperation::UploadOperation(LogWriter *logWriter,
                                       FileInfo fileToUpload,
                                       const ::file::path & pathToSourceRoot,
                                       const ::file::path & pathToTargetRoot)
      : CopyOperation(logWriter),
        m_pfileitem(0), m_preadable(0), m_gotoChild(false), m_gotoParent(false), m_firstUpload(true),
        //m_remoteFilesInfo(0), m_remoteFilesCount(0), m_bufferSize(20000)
      m_bufferSize(20000)
      {
         m_pathToSourceRoot= pathToSourceRoot;
         m_pathToTargetRoot= pathToTargetRoot;

         m_toCopy = new FileInfoList(logWriter, fileToUpload);

         changeFileToUpload(m_toCopy);
         //m_lastRequestTime = ::earth::time::now();
         m_lastRequestTime.Now();
      }

      UploadOperation::UploadOperation(LogWriter *logWriter,
                                       const ::pointer_array< FileInfo > & fileinfoa,
                                       const ::file::path & pathToSourceRoot,
                                       const ::file::path & pathToTargetRoot)
      : CopyOperation(logWriter),
        m_pfileitem(0), m_preadable(0), m_gotoChild(false), m_gotoParent(false), m_firstUpload(true)
        //m_remoteFilesInfo(0), m_remoteFilesCount(0), m_bufferSize(20000)
      , m_bufferSize(20000)
      {
         m_pathToSourceRoot= pathToSourceRoot;
         m_pathToTargetRoot= pathToTargetRoot;

         m_toCopy = new FileInfoList(logWriter, fileinfoa);

         changeFileToUpload(m_toCopy);
         //m_lastRequestTime = ::earth::time::now();
         m_lastRequestTime.Now();
      }

      UploadOperation::~UploadOperation()
      {
         if (m_toCopy != NULL) {
            delete m_toCopy->getRoot();
         }
         if (m_preadable != NULL) {
            try { m_preadable->close(); } catch (::io_exception) { }
            //delete m_preadable;
            m_preadable.release();
         }
         if (m_pfileitem != NULL) {
            //delete m_file;
            m_pfileitem.release();
         }
         releaseRemoteFilesInfo();
      }

      void UploadOperation::do_file_transfer_operation()
      {
         //
         // Reset flags
         //

         m_gotoChild = false;
         m_gotoParent = false;

         // Notify listeners that operation have started
         notifyStart();

         // Get total size of files to upload and null bytes processed count
         m_totalBytesToCopy = getInputFilesSize();
         m_totalBytesCopied = 0;

         //
         // Send file ::list_base request to know filelist of remote destination directory.
         // Real file upload will start when reply for this request will be received
         //

         m_firstUpload = true;

         m_sender->sendFileListRequest(m_pathToTargetRoot,
                                       m_replyBuffer->isCompressionSupported());
      }

      void UploadOperation::onUploadReply(DataInputStream * pinput)
      {
         sendFileDataChunk();
      }

      void UploadOperation::onUploadDataReply(DataInputStream * pinput)
      {
         if (isTerminating()) {
            gotoNext();
            return ;
         }

         sendFileDataChunk();
      }

      void UploadOperation::onUploadEndReply(DataInputStream * pinput)
      {
         // Cleanup
         try { m_preadable->close(); } catch (...) { }
         //delete m_preadable;
         //m_preadable = NULL;

         m_preadable.release();

         m_pfileitem.release();

         //delete m_pfileitem;
         //m_pfileitem = NULL;

         // Upload next file in the ::list_base
         gotoNext();
      }

      void UploadOperation::onMkdirReply(DataInputStream * pinput)
      {
         // Upload next file in the ::list_base
         gotoNext();
      }

      void UploadOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         //::string errDesc;

         auto errDesc = m_replyBuffer->getLastErrorMessage();

         notifyFailedToUpload(errDesc);

         //
         // If this LRF scopedstrMessage comes to file ::list_base request, then
         // don't need to upload next file, we must execute "special scopedstrMessage handler".
         //

         if (specialHandler()) {
            return ;
         }

         gotoNext();
      }

      void UploadOperation::onFileListReply(DataInputStream * pinput)
      {
         initRemoteFiles(m_replyBuffer->m_fileinfoa);
         specialHandler();
      }

      void UploadOperation::killOp()
      {
         //
         // If not files to upload, than clear memory and
         // operation is finished
         //

         delete m_toCopy->getRoot();
         m_toCopy = NULL;

         // Notify listeners that operation ended
         notifyFinish();
      }

      bool UploadOperation::specialHandler()
      {
         // If first upload that start real upload operation
         if (m_firstUpload) {
            m_firstUpload = false;
            startUpload();
            return true;
         }
         // Call real gotoNext method
         if (m_gotoChild) {
            m_gotoChild = false;
            gotoNext(false);
            return true;
         }
         if (m_gotoParent) {
            m_gotoParent = false;
            gotoNext(false);
            return true;
         }
         return false;
      }

      unsigned long long UploadOperation::getInputFilesSize()
      {
         unsigned long long totalFilesSize = 0;

         FileInfoList *fil = m_toCopy;
         while (fil != NULL) {
            //::string pathNoRoot;
            //::string pathToFile;

            auto pathNoRoot = fil->getAbsolutePath();

            auto pathToFile= m_pathToSourceRoot / pathNoRoot;

            totalFilesSize += getFileSize(pathToFile);

            fil = fil->getNext();
         }

         return totalFilesSize;

      }


      unsigned long long UploadOperation::getFileSize(const ::file::path & pathToFile)
      {
         unsigned long long fileSize = 0;

         auto pfileitem = m_logWriter->file_item(pathToFile);

         if (pfileitem->is_folder())
         {
            //::string_array stra;
            auto listing = pfileitem->list();

            //::string & fileNames = new ::string[filesCount];
            //pfileitem->list(fileNames, NULL);

            for (auto & path : listing)
            {

               auto pfileitemChild = file_item(path);

               //::string pathToSubFile;
               auto pathToSubFile = pfileitemChild->final_path();

               fileSize += pfileitemChild->size();
            }

            //delete[] fileNames;
         }
         else
         {

            long long len = pfileitem->size();

            if (len == -1)
            {
               fileSize = 0;
            }
            else
            {

               fileSize = (unsigned long long)len;

            }
         }

         return fileSize;

      }


      void UploadOperation::startUpload()
      {
         if (isTerminating()) {
            killOp();
            return ;
         } // terminate operation is needed

         // Current file info
         FileInfo *fileInfo = m_toCopy->getFileInfo();

         // Logging
         if (m_toCopy->getFirst()->getParent() == NULL) {
            ::string scopedstrMessage;

            scopedstrMessage.format("Uploading '{}' {}", m_pathToSourceFile,
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
         } // terminate operation is needed
      } // void

      void UploadOperation::processFolder()
      {
         ::string scopedstrMessage;

         // Try ::list_base files from folder
         FolderListener listener(m_logWriter, m_pathToSourceFile);
         if (listener.list()) {
            m_toCopy->setChild(m_logWriter, listener.m_fileinfoa);
         } else {
            // Logging
            ::string scopedstrMessage;

            scopedstrMessage.format("Error: failed to get file list in local folder '{}'",
                           m_pathToSourceFile);

            notifyError(scopedstrMessage);
         }

         // Send request to create folder
         m_sender->sendMkDirRequest(m_pathToTargetFile);
      }

      void UploadOperation::processFile()
      {
         //
         // Cleanup
         //

//         m_

         if (m_preadable != NULL) {
            try { m_preadable->close(); } catch (...) { }
            delete m_preadable;
         }
         if (m_pfileitem != NULL) {
            delete m_pfileitem;
         }

         unsigned long long initialFileOffset = 0;

         // Search if file already exists on remote machine
         for (unsigned int i = 0; i < m_fileinfoaRemote; i++) {
            FileInfo *localFileInfo = m_toCopy->getFileInfo();
            FileInfo *remoteFileInfo = m_fileinfoaRemote[i];

            auto strRemoteFileName = remoteFileInfo->getFileName();
            auto strLocalFileName = localFileInfo->getFileName();

            // ::file::item collision, show file exist dialog
            if (strLocalFileName == strRemoteFileName)
               {

               //
               // Copy listener must decide what to do with this situation
               //

               int action = m_copyListener->targetFileExists(localFileInfo,
                                                             remoteFileInfo,
                                                             m_pathToTargetFile);

               switch (action) {
                  case CopyFileEventListener::TFE_OVERWRITE:
                     break;
                  case CopyFileEventListener::TFE_APPEND:
                     initialFileOffset = remoteFileInfo->getSize();
                     m_totalBytesCopied += initialFileOffset;
                     break;
                  case CopyFileEventListener::TFE_SKIP:
                     m_totalBytesCopied += localFileInfo->getSize();
                     gotoNext();
                     return ;
                  case CopyFileEventListener::TFE_CANCEL:
                     if (!isTerminating()) {
                        terminate();
                     } // if not terminating
                     return ;
                  default: _ASSERT(FALSE);
               } // switch (action)
            } // if file already exists
         } // for all files in remote files

         // Trying to open file for reading
         m_pfileitem = file_item(m_pathToSourceFile);
         try {
            //::string path;
            auto path = m_pfileitem->final_path();
            m_preadable = file()->get_reader(path);
            // Try to seek
            m_preadable->set_position(initialFileOffset);

         } catch (::remoting::Exception &ioEx) {
            notifyFailedToUpload(ioEx.get_message());
            gotoNext();
            return ;
         } // try / catch

         bool overwrite = (initialFileOffset == 0);

         m_sender->sendUploadRequest(m_pathToTargetFile, overwrite,
                                     initialFileOffset);
      } // void

      void UploadOperation::sendFileDataChunk()
      {
         _ASSERT(m_preadable != NULL);

         if (m_lastRequestTime.elapsed() > 300_s) {
            m_bufferSize /= 2;
         }
         else {
            m_bufferSize *= 2;
         }
         if (m_bufferSize < 20000) {
            m_bufferSize = 20000;
         }
         if (m_bufferSize > 400000) {
            m_bufferSize = 400000;
         }
         m_lastRequestTime.Now();

         ::array_base<char> buffer(m_bufferSize);
         unsigned int read = 0;
         try {
            size_t portion = m_preadable->read(buffer.data(), m_bufferSize);
            _ASSERT((unsigned int)portion == portion);
            read = (unsigned int)portion;
         } catch (EOFException) {

            //
            // End of file.
            //

            m_preadable->close();

            class ::time lastModified;

            try {
               lastModified = m_pfileitem->last_modified_time();
            } catch (::io_exception) { } // try / catch

            m_sender->sendUploadEndRequest(0, lastModified.m_iSecond);
            return ;

         } catch (::io_exception &ioEx) {
            notifyFailedToUpload(ioEx.get_message());
            gotoNext();
            return ;
         } // try / catch

         try {
            m_sender->sendUploadDataRequest(buffer.data(), read, false);
            m_totalBytesCopied += read;

            // Notify listener, that data chunk is copied
            if (m_copyListener != NULL) {
               m_copyListener->dataChunkCopied(m_totalBytesCopied,
                                               m_totalBytesToCopy);
            }
         } catch (::io_exception &ioEx) {
            throw ioEx;
         } // try / catch
      }

      void UploadOperation::gotoNext()
      {
         gotoNext(true);
      }

      void UploadOperation::gotoNext(bool fake)
      {
         FileInfoList *current = m_toCopy;

         bool hasChild = current->getChild() != NULL;
         bool hasNext = current->getNext() != NULL;
         bool hasParent = current->getFirst()->getParent() != NULL;

         if (hasChild) {
            if (fake) {
               m_gotoChild = true;

               m_sender->sendFileListRequest(m_pathToTargetFile,
                                             m_replyBuffer->isCompressionSupported());
               return ;
            } else {
               // If it has child, we must upload child file ::list_base first
               changeFileToUpload(current->getChild());
               startUpload();
            }
         } else if (hasNext) {
            // If it has no child, but has next file, we must upload next file
            changeFileToUpload(current->getNext());
            startUpload();
         } else {

            //
            // If it has no child and not next, but has parent file,
            // we must go to parent file (folder i mean), set childs to NULL
            // cause we already upload child files and go to next file.
            //

            FileInfoList *first = current->getFirst();
            if (hasParent) {
               if (fake) {
                  m_gotoParent = true;

                  ::string pathToRemoteFolder(m_pathToTargetRoot);

                  FileInfoList *parentOfCurrent = m_toCopy->getFirst()->getParent();
                  FileInfoList *parentOfParent = NULL;

                  if (parentOfCurrent != NULL) {
                     parentOfParent = parentOfCurrent->getFirst()->getParent();
                  }

                  if (parentOfParent != NULL) {
                     pathToRemoteFolder = getRemotePath(parentOfParent, m_pathToTargetRoot);
                  }

                  m_sender->sendFileListRequest(pathToRemoteFolder,
                                                m_replyBuffer->isCompressionSupported());
                  return ;
               } else {
                  changeFileToUpload(first->getParent());
                  m_toCopy->setChild(nullptr,{});
                  gotoNext();
               }
            } else {
               killOp();
            } // if / else
         } // if / else
      } // void

      void UploadOperation::releaseRemoteFilesInfo()
      {
         m_fileinfoaRemote.clear();
         // if (m_fileinfoaRemote != NULL) {
         //    delete[] m_remoteFilesInfo;
         //    m_remoteFilesInfo = NULL;
         // }
         // m_remoteFilesCount = 0;
      }

      void UploadOperation::initRemoteFiles(const ::pointer_array<FileInfo> & fileinfoa)
      {
         releaseRemoteFilesInfo();

         m_fileinfoaRemote = fileinfoa;
         // m_remoteFilesCount = count;
         // m_remoteFilesInfo = new FileInfo[count];
         // for (unsigned int i = 0; i < count; i++) {
         //    m_remoteFilesInfo[i] = remoteFilesInfo[i];
         // }
      }

      void UploadOperation::notifyFailedToUpload(const ::scoped_string & scopedstrErrorDescription)
      {
         ::string scopedstrMessage;

         scopedstrMessage.format("Error: failed to upload '{}' {} ({})",
                        m_pathToSourceFile,
                        m_toCopy->getFileInfo()->isDirectory() ? "folder" : "file",
                        scopedstrErrorDescription);

         notifyError(scopedstrMessage);
      }

      void UploadOperation::changeFileToUpload(FileInfoList *toUpload)
      {
         m_toCopy = toUpload;

         m_pathToSourceFile = getLocalPath(m_toCopy, m_pathToSourceRoot);
         m_pathToTargetFile = getRemotePath(m_toCopy, m_pathToTargetRoot);
      }
   }
}