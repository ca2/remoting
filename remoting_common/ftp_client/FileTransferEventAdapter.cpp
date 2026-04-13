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
#include "framework.h"
#include "FileTransferEventAdapter.h"
#include "remoting/remoting_common/ftp_common/FTMessage.h"

namespace remoting
{
   namespace ftp
   {
      FileTransferEventAdapter::FileTransferEventAdapter()
      {
      }

      FileTransferEventAdapter::~FileTransferEventAdapter()
      {
      }

      void FileTransferEventAdapter::onCompressionSupportReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onFileListReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onMd5DataReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onUploadReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onUploadDataReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onUploadEndReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onDownloadReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onDownloadDataReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onDownloadEndReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onMkdirReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onRmReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onMvReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onDirSizeReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }

      void FileTransferEventAdapter::onLastRequestFailedReply(::subsystem::DataInputStream * pinput)
      {
         throw OperationNotPermittedException();
      }
   }
}