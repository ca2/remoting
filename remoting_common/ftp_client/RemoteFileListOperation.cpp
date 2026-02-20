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
#include "RemoteFileListOperation.h"
namespace remoting
{
   namespace ftp
   {
      RemoteFileListOperation::RemoteFileListOperation(LogWriter *logWriter,
                                                       const ::scoped_string & scopedstrRemotePath)
      : FileTransferOperation(logWriter),
        m_isOk(false),
        m_isFinished(false)
      {
         m_remotePath= scopedstrRemotePath;
      }

      RemoteFileListOperation::~RemoteFileListOperation()
      {
      }

      void RemoteFileListOperation::do_file_transfer_operation()
      {
         m_isOk = false;
         m_isFinished = false;

         m_sender->sendFileListRequest(m_remotePath,
                                       m_replyBuffer->isCompressionSupported());
         notifyStart();
      }

      void RemoteFileListOperation::onFileListReply(DataInputStream * pinput)
      {
         m_isOk = true;
         m_isFinished = true;
         notifyFinish();
      }

      void RemoteFileListOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         m_isOk = false;
         m_isFinished = true;

         // Logging
         ::string scopedstrMessage;

         scopedstrMessage.format("Error: failed to get file ::list_base in remote folder '{}'",
                        m_remotePath);

         notifyError(scopedstrMessage);

         // Notity listeners that operation has finished
         notifyFinish();
      }

      bool RemoteFileListOperation::isOk()
      {
         return m_isFinished && m_isOk;
      }
   }
}