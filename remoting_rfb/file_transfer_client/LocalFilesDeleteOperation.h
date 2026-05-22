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
#include "subsystem/thread/Thread.h"

namespace remoting_rfb
{
   namespace file_transfer
   {
      class CLASS_DECL_REMOTING_RFB LocalFilesDeleteOperation : public FileTransferOperation
      {
      public:
         LocalFilesDeleteOperation(::subsystem::LogWriter * plogwriter,
                                  const ::pointer_array<FileInfo> & fileinfoa,
                                   const ::file::path & pathToTargetRoot);
         ~LocalFilesDeleteOperation() override;

         //virtual void start();
         //virtual void setThreadToFinish();

         //private:

         void do_file_transfer_operation() override;

         bool deleteFile(::file::item *file);

         //protected:
         ::file::path m_pathToTargetRoot;
         ::pointer_array<FileInfo> m_fileinfoaDelete;
         //::u32 m_filesCount;
      };
   }
}