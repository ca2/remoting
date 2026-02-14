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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "RemoteFolderCreateOperation.h"

RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                         const ::scoped_string & scopedstrPathToTargetFile)
: FileTransferOperation(logWriter)
{
  m_pathToTargetFile= pathToTargetFile;
}

RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                         FileInfo file,
                                                         const ::scoped_string & scopedstrPathToTargetRoot)
: FileTransferOperation(logWriter)
{
  FileInfoList *plist = new FileInfoList(file);
  getRemotePath(plist, pathToTargetRoot, &m_pathToTargetFile);
  // or delete plist->getRoot(); ?
  delete plist;
}

RemoteFolderCreateOperation::~RemoteFolderCreateOperation()
{
}

void RemoteFolderCreateOperation::start()
{
  // Logging
  ::string message;

  message.formatf("Creating remote folder '{}'",
                 m_pathToTargetFile);

  notifyInformation(message);

  // Nofity all that operation have started
  notifyStart();

  // Send mkdir request to server
  m_sender->sendMkDirRequest(m_pathToTargetFile);
}

void RemoteFolderCreateOperation::onMkdirReply(DataInputStream *input)
{
  // Notify all that operation have ended
  notifyFinish();
}

void RemoteFolderCreateOperation::onLastRequestFailedReply(DataInputStream *input)
{
  // Logging
  ::string message;

  message.formatf("Error: failed to create remote folder '{}'",
                 m_pathToTargetFile);

  notifyError(message);

  // Notify all that operation have ended
  notifyFinish();
}
