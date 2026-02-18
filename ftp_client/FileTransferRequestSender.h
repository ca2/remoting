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

#pragma once


#include "util/inttypes.h"
#include "network/RfbOutputGate.h"
#include "io_lib/io_exception.h"

//#include "log_writer/LogWriter.h"


namespace remoting
{
   namespace ftp
   {
      class FileTransferRequestSender
      {
      public:
         FileTransferRequestSender(LogWriter *logWriter);
         ~FileTransferRequestSender();

         void setOutput(RfbOutputGate *outputStream);

         void sendCompressionSupportRequest();
         void sendFileListRequest(const ::scoped_string & scopedstrFullPath, bool useCompression);
         void sendDownloadRequest(const ::scoped_string & scopedstrFullPathName, unsigned long long offset);
         void sendDownloadDataRequest(unsigned int size, bool useCompression);
         void sendRmFileRequest(const ::scoped_string & scopedstrFullPathName);
         void sendMkDirRequest(const ::scoped_string & scopedstrFullPathName);
         void sendMvFileRequest(const ::scoped_string & scopedstroldFileName, const ::scoped_string & scopedstrNewFileName);
         void sendUploadRequest(const ::scoped_string & scopedstrFullPathName, bool overwrite, unsigned long long offset);
         void sendUploadDataRequest(const char *buffer, unsigned int size, bool useCompression);
         void sendUploadEndRequest(unsigned char fileFlags, unsigned long long modificationTime);
         void sendFolderSizeRequest(const ::scoped_string & scopedstrFullPath);

      protected:
         LogWriter *m_logWriter;
         RfbOutputGate *m_output;
      };
   }
}