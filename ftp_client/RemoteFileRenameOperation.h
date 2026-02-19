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

//#pragma once
//
#pragma once

#include "FileTransferOperation.h"

namespace remoting
{
   namespace ftp
   {
      //
      // Class used for renaming files on remote file system.
      //

      class RemoteFileRenameOperation : public FileTransferOperation
      {
      public:
         RemoteFileRenameOperation(LogWriter *logWriter,
                                   const ::file::path & pathToSourceFile,
                                   const ::file::path & pathToTargetFile);

         RemoteFileRenameOperation(LogWriter *logWriter,
                                   FileInfo sourceFileInfo,
                                   FileInfo targetFileInfo,
                                   const ::file::path & pathToTargetRoot);

         virtual ~RemoteFileRenameOperation();

         void do_file_transfer_operation() override;

         //protected:
         virtual void onMvReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);


         ::file::path m_pathToSourceFile;
         ::file::path m_pathToTargetFile;
      };

      //
   }
}