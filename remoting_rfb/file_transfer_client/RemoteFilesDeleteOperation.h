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


#include "FileTransferOperation.h"
#include "FileInfoList.h"

namespace remoting_rfb
{
   namespace file_transfer
   {
      class RemoteFilesDeleteOperation : public FileTransferOperation
      {
      public:

         //
         // Constructs new RemoteFilesDeleteOperation class instance.
         //
         // Parameters:
         //
         // fileInfoToDelete - file information about file need to delete.
         // filename in this variable must be relative to pathToTargetRoot.
         //
         // pathToTargetRoot - path to folder on remote file system in ft format where
         // files to be deleted is located.
         //

         RemoteFilesDeleteOperation(::subsystem::LogWriter * plogwriter,
                                    const ::pointer_array< FileInfo > & fileinfoa,
                                    const ::file::path & pathToTargetRoot);

         RemoteFilesDeleteOperation(::subsystem::LogWriter * plogwriter,
                                    FileInfo fileInfoToDelete,
                                    const ::file::path & pathToTargetRoot);

         virtual ~RemoteFilesDeleteOperation();

         //
         // Starts executing this delete operation
         //

         void do_file_transfer_operation() override;

         //
         // ::file::item transfer scopedstrMessage(accepted by this operation) handlers
         //

         void onFileListReply(::DataInputStream * pinput) override;
         void onRmReply(::DataInputStream * pinput) override;
         void onLastRequestFailedReply(::DataInputStream * pinput) override;

      //private:

         void remove(bool removeIfFolder);
         void gotoNext();
         void killOp();

      //protected:

         //
         // Current file ::list_base to delete
         //

         FileInfoList *m_toDelete;
         ::file::path m_pathToTargetRoot;
      };
   }
}
