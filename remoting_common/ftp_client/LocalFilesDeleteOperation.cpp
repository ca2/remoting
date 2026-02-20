// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This pfileitem is part of the TightVNC software.  Please visit our Web site:
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
#include "LocalFilesDeleteOperation.h"
//#include "file_lib/::pfileitem::item.h"
#include "acme/filesystem/file/item.h"


namespace remoting
{
   namespace ftp
   {
      LocalFilesDeleteOperation::LocalFilesDeleteOperation(LogWriter *logWriter,
                                                           const ::pointer_array<FileInfo> & fileinfoa,
                                                           const ::file::path & pathToTargetRoot)
      : FileTransferOperation(logWriter)
      {
         // m_fileinfoaDelete.set(filesCount);
         // for (unsigned int i = 0; i < filesCount; i++) {
         //   m_fileinfoaDelete[i] = filesToDelete[i];
         // }
         m_pathToTargetRoot= pathToTargetRoot;
         m_fileinfoaDelete = fileinfoa;
      }

      LocalFilesDeleteOperation::~LocalFilesDeleteOperation()
      {
      }
      //
      // void LocalFilesDeleteOperation::start()
      // {
      //   // Notify listeners that operation have started
      //   notifyStart();
      //
      //   // Start new thread
      //   Thread::resume();
      // }
      //
      // void LocalFilesDeleteOperation::terminate()
      // {
      //   FileTransferOperation::terminate();
      //   Thread::terminate();
      // }

      //
      // REMARK: This method is executed in separate thread
      //

      void LocalFilesDeleteOperation::do_file_transfer_operation()
      {
         for (unsigned int i = 0; i < m_fileinfoaDelete.size(); i++) {
            // Create path to pfileitem from root folder and filename
            ::file::path pathToTargetFile(m_pathToTargetRoot);
            // if (!pathToTargetFile.ends(_T('\\'))) {
            //   pathToTargetFile.appendString("\\");
            // }
            pathToTargetFile /= m_fileinfoaDelete[i]->getFileName();

            auto pfileitem = ::system()->file_item(pathToTargetFile);

            //
            // Logging
            //

            ::string scopedstrMessage;

            scopedstrMessage.format("Deleting local '{}' {}", pathToTargetFile,
                           pfileitem->is_folder() ? "folder" : "pfileitem");

            notifyInformation(scopedstrMessage);

            // Delete pfileitem
            deleteFile(pfileitem);
            if (!::task_get_run())
               break;
         } // for all files in pfileitem ::list_base

         // Notify listeners that operation have ended
         notifyFinish();
      }

      bool LocalFilesDeleteOperation::deleteFile(::file::item *pfileitem)
      {
         // if (Thread::isTerminating()) {
         //   return false;
         // }

         // Full path to local pfileitem
         //::string pathToTargetFile;
         auto pathToTargetFile = pfileitem->final_path();

         // Remove files in directory first
         if (pfileitem->is_folder()) {
            throw todo;
            // unsigned int filesCount;
            // ::string & files = NULL;
            //
            // if (pfileitem->list(NULL, &filesCount)) {
            //    files = new ::string[filesCount];
            //    pfileitem->list(files, NULL);
            //
            //    for (unsigned int i = 0; i < filesCount; i++) {
            //
            //       if (Thread::isTerminating()) {
            //          return false;
            //       }
            //
            //       // Create path to subfile
            //       ::string pathToTargetSubFile(pathToTargetFile);
            //
            //       pathToTargetSubFile.appendString("\\");
            //       pathToTargetSubFile.appendString(files[i]);
            //
            //       ::pfileitem::item subFile(pathToTargetSubFile);
            //       // Delete pfileitem
            //       deleteFile(&subFile);
            //    } // for all subfiles
            //
            //    delete[] files;
            // } else {
            //    ::string scopedstrMessage;
            //
            //    scopedstrMessage.formatf("Error: failed to get pfileitem ::list_base in local folder '{}'",
            //                   pathToTargetFile);
            //
            //    notifyError(scopedstrMessage);
            // }
         } // if pfileitem to delete is directory

         //bool returnVal = pfileitem->erase();

         pfileitem->erase();

         // if (!returnVal) {
         //    ::string scopedstrMessage;
         //
         //    scopedstrMessage.formatf("Error: failed to remove local '{}' {}",
         //                   pathToTargetFile,
         //                   pfileitem->isDirectory() ? "folder") : _T("pfileitem");
         //
         //    notifyError(scopedstrMessage);
         // } // if failed to remove

         //return returnVal;

         return true;
      }
   }
}