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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


//#include "file_lib/::file::item.h"
//#include "file_lib/WinFileChannel.h"
#include "FileTransferOperation.h"
#include "FileInfoList.h"
#include "CopyOperation.h"

//
// ::file::item transfer operation class for uploading files (and file trees).
//
//
// Remark: Class uses difficult trick to get remote directory file ::list_base
// (to check file collisions, when we trying to upload some file,
// but it's already exist on remote machine). 
//
// So, we need to send file ::list_base request every time before step one folder down
// or up (as you know, all this features realized in 'gotoNext' method) and when reply
// for this request (FLR) is received, we must execute old piece of code from
// 'gotoNext' method to really step one folder down or up.
//
// Also we must do this trick in 'start' method, cause we cannot start upload
// immedianly. We can start uploading files only when we will know remote
// destination folder's filelist.
//
#pragma once

namespace remoting
{
   namespace ftp
   {
      class CLASS_DECL_REMOTING_COMMON UploadOperation : public CopyOperation
      {
      public:

         //
         // Arguments:
         //
         // fileToUpload - information about file to upload, filename in this class
         // must be relative (only filename, not full path to file).
         //
         // pathToSourceRoot - absolute path on local file system to folder where files to upload
         // is located (system depended).
         //
         // pathToTargetRoot - absolute path to destination directory where uploaded file tree
         // must be located on remote file system after upload (system independed, in file transfer
         // filename format, see file transfer protocol extension for more information).
         //

         UploadOperation(LogWriter *logWriter,
                         FileInfo fileToUpload,
                         const ::file::path & pathToSourceRoot,
                         const ::file::path & pathToTargetRoot);

         UploadOperation(LogWriter *logWriter,
                         const ::pointer_array < FileInfo > & fileinfoa,
                         const ::file::path & pathToSourceRoot,
                         const ::file::path & pathToTargetRoot);

         virtual ~UploadOperation();

         //
         // Starts upload operation
         //

         void do_file_transfer_operation() override;

         //
         // Inherited from FileTransferEventHandler class
         //
         // Event handlers to process file transfer messages
         //

         virtual void onUploadReply(DataInputStream * pinput);
         virtual void onUploadDataReply(DataInputStream * pinput);
         virtual void onUploadEndReply(DataInputStream * pinput);
         virtual void onMkdirReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);
         virtual void onFileListReply(DataInputStream * pinput);

      //private:

         unsigned long long getInputFilesSize();
         unsigned long long getFileSize(const ::file::path & pathToFile);

      //protected:

         // Terminates operation execution
         void killOp();

         //
         // Special ft scopedstrMessage handler for "Last request failed" and "::file::item ::list_base request".
         //

         bool specialHandler();

         //
         // Start current file upload (current file is notated by m_toCopy value).
         //

         void startUpload();

         //
         // Lists folder from local file system and set this ::list_base as child to current
         // file ::list_base (m_toCopy), after than it tryies to create folder on remote
         // file system.
         //

         void processFolder();

         //
         // Tryies to start upload of current file (notated by m_toCopy member).
         //

         void processFile();

         //
         // Calls gotoNext method with true 'fake' argument
         //

         void gotoNext();

         //
         // Choose next file to upload from m_toCopy ::list_base to m_toCopy ::list_base.
         //
         // Remark: Also made some magic things with m_toCopy ::list_base to avoid "infinity" loops.
         // Remark: if fake is true (it's true by default) method does not change current file
         // to child or parent, and sets 'Get file ::list_base trick' flags and sends file ::list_base request.
         //

         void gotoNext(bool fake);

         //
         // Reads data chunk from current uploading file and
         // sends it to server.
         //

         void sendFileDataChunk();

         //
         // Helper methods to control m_remoteFilesInfo, m_remoteFilesCount
         // members.
         //

         void releaseRemoteFilesInfo();
         void initRemoteFiles(const ::pointer_array < FileInfo > & fileinfoa);

         // Helper method that creates scopedstrMessage and notify listeners
         void notifyFailedToUpload(const ::scoped_string & scopedstrErrorDescription);

         // Changed m_toCopy member and updates m_pathToSourceFile,
         // m_pathToTargetFile members
         void changeFileToUpload(FileInfoList *toUpload);

      //public:
         // Source file that we uploading now
         ::pointer < ::file::item > m_pfileitem;
         // ::file::item input stream associated with m_file
         //WinFileChannel *m_pbinarystream;

         //::pointer < ::binary_stream > m_pbinarystream;


         ::pointer < ::file::readable > m_preadable;

         // ::file::item ::list_base of remote directory where we uploading
         // current file now
         //FileInfo *m_remoteFilesInfo;
         //unsigned int m_remoteFilesCount;

         ::pointer_array< ::remoting::ftp::FileInfo>m_fileinfoaRemote;

         //
         // Helper members of 'Remote file ::list_base trick'
         //

         bool m_gotoChild;
         bool m_gotoParent;
         bool m_firstUpload;

         // request data size changes dynamicaly depends on request rate
         size_t m_bufferSize;
         class ::time m_lastRequestTime;

      };
   }
}
