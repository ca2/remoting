// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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


////#include "subsystem/platform/inttypes.h"
#include "remoting/remoting_rtc/network/RfbOutputGate.h"
#include "acme/exception/io.h"

//#include "log_writer/LogWriter.h"


namespace remoting
{
   namespace file_transfer
   {
      class CLASS_DECL_REMOTING FileTransferRequestSender
      {
      public:
         FileTransferRequestSender(::subsystem::LogWriter * plogwriter);
         ~FileTransferRequestSender();

         void setOutput(::remoting_rtc::RfbOutputGate *outputStream);

         void sendCompressionSupportRequest();
         void sendFileListRequest(const ::scoped_string & scopedstrFullPath, bool useCompression);
         void sendDownloadRequest(const ::scoped_string & scopedstrFullPathName, ::u64 offset);
         void sendDownloadDataRequest(::u32 size, bool useCompression);
         void sendRmFileRequest(const ::scoped_string & scopedstrFullPathName);
         void sendMkDirRequest(const ::scoped_string & scopedstrFullPathName);
         void sendMvFileRequest(const ::scoped_string & scopedstroldFileName, const ::scoped_string & scopedstrNewFileName);
         void sendUploadRequest(const ::scoped_string & scopedstrFullPathName, bool overwrite, ::u64 offset);
         void sendUploadDataRequest(const_char_pointer pszBuffer, ::u32 size, bool useCompression);
         void sendUploadEndRequest(::u8 fileFlags, ::u64 modificationTime);
         void sendFolderSizeRequest(const ::scoped_string & scopedstrFullPath);

      protected:
         ::pointer < ::subsystem::LogWriter > m_plogwriter;
         ::remoting_rtc::RfbOutputGate *m_output;
      };
   }
}