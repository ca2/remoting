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
#include "RemoteFileRenameOperation.h"

RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                     const ::scoped_string & scopedstrPathToSourceFile,
                                                     const ::scoped_string & scopedstrPathToTargetFile)
: FileTransferOperation(logWriter)
{
  m_pathToSourceFile= scopedstrPathToSourceFile;
  m_pathToTargetFile= scopedstrPathToTargetFile;
}

RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                     FileInfo sourceFileInfo,
                                                     FileInfo targetFileInfo,
                                                     const ::scoped_string & scopedstrPathToTargetRoot)
: FileTransferOperation(logWriter)
{
  FileInfoList srcList(sourceFileInfo);
  FileInfoList dstList(targetFileInfo);

  getRemotePath(&srcList, scopedstrPathToTargetRoot, m_pathToSourceFile);
  getRemotePath(&dstList, scopedstrPathToTargetRoot, m_pathToTargetFile);
}

RemoteFileRenameOperation::~RemoteFileRenameOperation()
{
}

void RemoteFileRenameOperation::start()
{
  // Logging
  ::string message;

  message.formatf("Renaming remote file '{}' to '{}'",
                 m_pathToSourceFile,
                 m_pathToTargetFile);

  notifyInformation(message);

  // Notify all that operation have started
  notifyStart();

  // Send request to server
  m_sender->sendMvFileRequest(m_pathToSourceFile,
                              m_pathToTargetFile);
}

void RemoteFileRenameOperation::onMvReply(DataInputStream *input)
{
  // Notify listeners that operation has finished
  notifyFinish();
}

void RemoteFileRenameOperation::onLastRequestFailedReply(DataInputStream *input)
{
  // Logging
  ::string message;

  message.formatf("Error: failed to rename remote '{}' file",
                 m_pathToSourceFile);

  notifyInformation(message);

  // Notify listeners that operation has finished
  notifyFinish();
}
