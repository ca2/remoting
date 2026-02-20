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


#include "remoting/remoting_common/ftp_common/FileInfo.h"
//#include "file_lib/WinFileChannel.h"
#include "FileInfoList.h"
#include "CopyOperation.h"


namespace remoting
{
   namespace ftp
   {
      //
      // ::file::item transfer operation class for downloading files (and file trees).
      //

      class CLASS_DECL_REMOTING_COMMON DownloadOperation : public CopyOperation
      {
      public:

         //
         // Parameters:
         //
         // [IN] filesToDownload - array of files info to be downloaded
         // [IN] filesCount - count elements in filesToDownload array
         // [IN] pathToTargetRoot - full path to local folder where downloaded files
         // will be located (system depended).
         // [IN] pathToSourceRoot - full path to remote folder where files to download
         // is located (system independed, see ft protocol info)
         //

         DownloadOperation(LogWriter *logWriter,
                           const ::pointer_array< FileInfo >  & fileinfoa,
                           const ::file::path & pathToTargetRoot,
                           const ::file::path & pathToSourceRoot);

         virtual ~DownloadOperation();

         //
         // Inherited from FileTransferOperation
         //

         void do_file_transfer_operation() override;

      //protected:

         //
         // Inherited from FileTransferEventHandler
         //

         virtual void onFileListReply(DataInputStream * pinput);
         virtual void onDownloadReply(DataInputStream * pinput);
         virtual void onDownloadDataReply(DataInputStream * pinput);
         virtual void onDownloadEndReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);
         virtual void onDirSizeReply(DataInputStream * pinput);

      //private:

         // Starts download of current file (m_toCopy member)
         void startDownload();

         // Starts download of file
         void processFile();

         // Start download of folder
         void processFolder();

         // Sets m_toCopy member to next file to download
         void gotoNext();

         // Sends get folder size request to server to know
         // how many bytes must be receivied during
         // all download process
         void tryCalcInputFilesSize();

         // Terminates operation execution
         void killOp();

         // Helper method that decrements m_foldersToCalcSizeLeft count
         // and if it's equal to null, start first file download
         void decFoldersToCalcSizeCount();

         // Helper method that creates scopedstrMessage string and notifies listeners
         void notifyFailedToDownload(const ::scoped_string & scopedstrErrorDescription);

         // Sets current m_toCopy member value and updates
         // m_pathToSourceFile, m_pathToTargetFile members
         void changeFileToDownload(FileInfoList *toDownload);

      //protected:
         // Target local file
         ::pointer < ::file::item >m_pfileitem;
         // ::file::item output stream associated with m_file
          //::pointer < ::file::stream> m_pstream;

         ::pointer < ::file::writable > m_pwriteable;

         // Initial file offset for current download (broken downloads)
         unsigned long long m_fileOffset;

         // Helper member to know how many folders to download left
         // to get their file size
         unsigned int m_foldersToCalcSizeLeft;

         // request data size changes dynamicaly depends on request rate
         size_t m_bufferSize;
         ::earth::time m_lastRequestTime;

      };
   } // namespace ftp
} // namespace remoting