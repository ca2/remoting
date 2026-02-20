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
#include "RemoteFileRenameOperation.h"

namespace remoting
{
   namespace ftp
   {
      RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                           const ::file::path & pathToSourceFile,
                                                           const ::file::path & pathToTargetFile)
      : FileTransferOperation(logWriter)
      {
         m_pathToSourceFile= pathToSourceFile;
         m_pathToTargetFile= pathToTargetFile;
      }

      RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                           FileInfo sourceFileInfo,
                                                           FileInfo targetFileInfo,
                                                           const ::file::path & pathToTargetRoot)
      : FileTransferOperation(logWriter)
      {
         FileInfoList srcList(logWriter, sourceFileInfo);
         FileInfoList dstList(logWriter, targetFileInfo);

         m_pathToSourceFile = getRemotePath(&srcList, pathToTargetRoot);
         m_pathToTargetFile = getRemotePath(&dstList, pathToTargetRoot);
      }

      RemoteFileRenameOperation::~RemoteFileRenameOperation()
      {
      }

      void RemoteFileRenameOperation::do_file_transfer_operation()
      {
         // Logging
         ::string scopedstrMessage;

         scopedstrMessage.format("Renaming remote file '{}' to '{}'",
                        m_pathToSourceFile,
                        m_pathToTargetFile);

         notifyInformation(scopedstrMessage);

         // Notify all that operation have started
         notifyStart();

         // Send request to server
         m_sender->sendMvFileRequest(m_pathToSourceFile,
                                     m_pathToTargetFile);
      }

      void RemoteFileRenameOperation::onMvReply(DataInputStream * pinput)
      {
         // Notify listeners that operation has finished
         notifyFinish();
      }

      void RemoteFileRenameOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         // Logging
         ::string scopedstrMessage;

         scopedstrMessage.formatf("Error: failed to rename remote '{}' file",
                        m_pathToSourceFile);

         notifyInformation(scopedstrMessage);

         // Notify listeners that operation has finished
         notifyFinish();
      }
   }
}