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


//
// Abstract class with abstract methods that must be
// overrided by child classes to process file transfer
// extension messages.
//
// FIXME: It can be renamed to FileTransferMessageListener
//
#pragma once


#include "acme/subsystem/io/DataInputStream.h"

namespace remoting
{
   namespace ftp
   {
      class CLASS_DECL_REMOTING_COMMON FileTransferEventHandler :
      virtual public ::particle
      {
      public:
         FileTransferEventHandler();
         virtual ~FileTransferEventHandler();

         virtual void onCompressionSupportReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onFileListReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onMd5DataReply(::subsystem::DataInputStream * pinput) = 0;

         virtual void onUploadReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onUploadDataReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onUploadEndReply(::subsystem::DataInputStream * pinput) = 0;

         virtual void onDownloadReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onDownloadDataReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onDownloadEndReply(::subsystem::DataInputStream * pinput) = 0;

         virtual void onMkdirReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onRmReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onMvReply(::subsystem::DataInputStream * pinput) = 0;

         virtual void onDirSizeReply(::subsystem::DataInputStream * pinput) = 0;
         virtual void onLastRequestFailedReply(::subsystem::DataInputStream * pinput) = 0;
      };
   }
}