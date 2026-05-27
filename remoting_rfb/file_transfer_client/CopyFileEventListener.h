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


//#include "subsystem/platform/inttypes.h"
#include "remoting/remoting_rfb/file_transfer_common/FileInfo.h"

namespace remoting_rfb
{
   namespace file_transfer
   {
      class CLASS_DECL_REMOTING_RFB CopyFileEventListener
      {
      public:

         friend class CopyOperation;
         friend class UploadOperation;
         friend class DownloadOperation;

         static const ::i32 TFE_OVERWRITE = 0x0;
         static const ::i32 TFE_SKIP      = 0x1;
         static const ::i32 TFE_APPEND    = 0x2;
         static const ::i32 TFE_CANCEL    = 0x3;

      protected:

         //
         // Method called by copy operation when data chunk of source file is copied
         // to target file.
         //
         // Parameters:
         //
         // totalBytesCopied - total length of data in bytes that copied to target
         // file system during copy operation execution
         //
         // totalBytesToCopy - total length of data in bytes that must be copied to target
         // file system during copy operation execution
         //

         virtual void dataChunkCopied(::u64 totalBytesCopied, ::u64 totalBytesToCopy) = 0;

         //
         // Method called by copy operation when some file must be copied,
         // but target file with such name already exists and operation doesn't
         // know what to do file target file.
         //
         // Next operation activity depends on this method returning value.
         //
         // Valid returning values:
         //
         // TFE_OVERWRITE - overwrite
         // TFE_SKIP - skip
         // TFE_APPEND - append
         // TFE_CANCEL - setThreadToFinish operation
         //
         // Remark: method must return valid value (see above), or assertion will
         // fail.
         //

         virtual ::i32 targetFileExists(FileInfo *sourceFileInfo,
                                      FileInfo *targetFileInfo,
                                      const ::file::path & pathToTargetFile) = 0;
      };
   }
}