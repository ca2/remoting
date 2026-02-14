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
#include "LocalFilesDeleteOperation.h"
#include "file_lib/File.h"

LocalFilesDeleteOperation::LocalFilesDeleteOperation(LogWriter *logWriter,
                                                     const FileInfo *filesToDelete,
                                                     unsigned int filesCount,
                                                     const ::scoped_string & pathToTargetRoot)
: FileTransferOperation(logWriter)
{
  m_filesToDelete.resize(filesCount);
  for (unsigned int i = 0; i < filesCount; i++) {
    m_filesToDelete[i] = filesToDelete[i];
  }
  m_pathToTargetRoot= pathToTargetRoot;
}

LocalFilesDeleteOperation::~LocalFilesDeleteOperation()
{
}

void LocalFilesDeleteOperation::start()
{
  // Notify listeners that operation have started
  notifyStart();

  // Start new thread
  Thread::resume();
}

void LocalFilesDeleteOperation::terminate()
{
  FileTransferOperation::terminate();
  Thread::terminate();
}

//
// REMARK: This method is executed in separate thread
//

void LocalFilesDeleteOperation::execute()
{
  for (unsigned int i = 0; i < m_filesToDelete.size(); i++) {
    // Create path to file from root folder and filename
    ::string pathToTargetFile(m_pathToTargetRoot);
    if (!pathToTargetFile.endsWith(_T('\\'))) {
      pathToTargetFile.appendString("\\");
    }
    pathToTargetFile.appendString(m_filesToDelete[i].getFileName());

    File file(pathToTargetFile);

    //
    // Logging
    //

    ::string message;

    message.formatf("Deleting local '{}' {}", pathToTargetFile,
                   file.isDirectory() ? "folder") : _T("file");

    notifyInformation(message);

    // Delete file
    deleteFile(&file);
  } // for all files in file ::list

  // Notify listeners that operation have ended
  notifyFinish();
}

bool LocalFilesDeleteOperation::deleteFile(File *file)
{
  if (Thread::isTerminating()) {
    return false;
  }

  // Full path to local file
  ::string pathToTargetFile;
  file->getPath(&pathToTargetFile);

  // Remove files in directory first
  if (file->isDirectory()) {
    unsigned int filesCount;
    ::string & files = NULL;

    if (file->list(NULL, &filesCount)) {
      files = new ::string[filesCount];
      file->list(files, NULL);

      for (unsigned int i = 0; i < filesCount; i++) {

        if (Thread::isTerminating()) {
          return false;
        }

        // Create path to subfile
        ::string pathToTargetSubFile(pathToTargetFile);

        pathToTargetSubFile.appendString("\\");
        pathToTargetSubFile.appendString(files[i]);

        File subFile(pathToTargetSubFile);
        // Delete file
        deleteFile(&subFile);
      } // for all subfiles

      delete[] files;
    } else {
      ::string message;

      message.formatf("Error: failed to get file ::list in local folder '{}'",
                     pathToTargetFile);

      notifyError(message);
    }
  } // if file to delete is directory

  bool returnVal = file->remove();

  if (!returnVal) {
    ::string message;

    message.formatf("Error: failed to remove local '{}' {}",
                   pathToTargetFile,
                   file->isDirectory() ? "folder") : _T("file");

    notifyError(message);
  } // if failed to remove

  return returnVal;
}
