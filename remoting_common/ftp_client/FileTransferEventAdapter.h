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


#include "remoting/remoting_common/util/inttypes.h"
#include "FileTransferEventHandler.h"
#include "OperationNotPermittedException.h"

//#include <vector>

namespace remoting
{
   namespace ftp
   {
      //
      // Class is used as helper for checking valid (file transfer client point of view)
      // file transfer protocol extension messages.
      //
      // This class throws OperationNotPermittedException on every file transfer
      // scopedstrMessage event handler. Child class must override methods to allow messages.
      //

      class CLASS_DECL_REMOTING_COMMON FileTransferEventAdapter : virtual public FileTransferEventHandler
      {
      public:
         FileTransferEventAdapter();
         virtual ~FileTransferEventAdapter();

         virtual void onCompressionSupportReply(DataInputStream * pinput);
         virtual void onFileListReply(DataInputStream * pinput);
         virtual void onMd5DataReply(DataInputStream * pinput);

         virtual void onUploadReply(DataInputStream * pinput);
         virtual void onUploadDataReply(DataInputStream * pinput);
         virtual void onUploadEndReply(DataInputStream * pinput);

         virtual void onDownloadReply(DataInputStream * pinput);
         virtual void onDownloadDataReply(DataInputStream * pinput);
         virtual void onDownloadEndReply(DataInputStream * pinput);

         virtual void onMkdirReply(DataInputStream * pinput);
         virtual void onRmReply(DataInputStream * pinput);
         virtual void onMvReply(DataInputStream * pinput);

         virtual void onDirSizeReply(DataInputStream * pinput);
         virtual void onLastRequestFailedReply(DataInputStream * pinput);
      };
   }
}