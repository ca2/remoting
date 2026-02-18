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


#include "FileTransferOperation.h"

namespace remoting
{
   namespace ftp
   {
      //
      // ::file::item operation that used for receiving file ::list_base
      // from remote file system.
      //

      class RemoteFileListOperation : public FileTransferOperation
      {
      public:
         RemoteFileListOperation(LogWriter *logWriter, const ::scoped_string & scopedstrRemotePath);
         virtual ~RemoteFileListOperation();

         //
         // Methods inherited from FileTransferOperation class
         //

         void do_file_transfer_operation() override;

         //
         // ::file::item transfer scopedstrMessage handlers
         //

         virtual void onFileListReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);

         //
         // Returns true if operation is finished and was no error,
         // false otherwise
         //

         bool isOk();

      //public:
         ::string m_remotePath;

         bool m_isFinished;
         bool m_isOk;
      };
   }
}