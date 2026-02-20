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
#include "RemoteFolderCreateOperation.h"


namespace remoting
{
   namespace ftp
   {
      RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                               const ::file::path & pathToTargetFile)
      : FileTransferOperation(logWriter)
      {
         m_pathToTargetFile = pathToTargetFile;
      }

      RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                               FileInfo file,
                                                               const ::file::path & pathToTargetRoot)
      : FileTransferOperation(logWriter)
      {
         FileInfoList *plist = new FileInfoList(logWriter, file);
         m_pathToTargetFile = getRemotePath(plist, pathToTargetRoot);
         // or delete plist->getRoot(); ?
         delete plist;
      }

      RemoteFolderCreateOperation::~RemoteFolderCreateOperation()
      {
      }

      void RemoteFolderCreateOperation::do_file_transfer_operation()
      {
         // Logging
         ::string scopedstrMessage;

         scopedstrMessage.formatf("Creating remote folder '{}'",
                        m_pathToTargetFile);

         notifyInformation(scopedstrMessage);

         // Nofity all that operation have started
         notifyStart();

         // Send mkdir request to server
         m_sender->sendMkDirRequest(m_pathToTargetFile);
      }

      void RemoteFolderCreateOperation::onMkdirReply(DataInputStream * pinput)
      {
         // Notify all that operation have ended
         notifyFinish();
      }

      void RemoteFolderCreateOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         // Logging
         ::string scopedstrMessage;

         scopedstrMessage.format("Error: failed to create remote folder '{}'",
                        m_pathToTargetFile);

         notifyError(scopedstrMessage);

         // Notify all that operation have ended
         notifyFinish();
      }
   }
}