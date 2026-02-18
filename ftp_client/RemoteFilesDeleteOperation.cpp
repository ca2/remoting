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
#include "RemoteFilesDeleteOperation.h"

namespace remoting
{
   namespace ftp
   {
      RemoteFilesDeleteOperation::RemoteFilesDeleteOperation(LogWriter *logWriter,
                                                             const ::pointer_array< FileInfo > & fileinfoa,
                                                             const ::file::path & pathToTargetRoot)
      : FileTransferOperation(logWriter)
      {
         m_toDelete = new FileInfoList(logWriter, fileinfoa);
         m_pathToTargetRoot= pathToTargetRoot;
      }

      RemoteFilesDeleteOperation::RemoteFilesDeleteOperation(LogWriter *logWriter,
                                                             FileInfo fileInfoToDelete,
                                                             const ::file::path & pathToTargetRoot)
      : FileTransferOperation(logWriter)
      {
         m_toDelete = new FileInfoList(logWriter, fileInfoToDelete);
         m_pathToTargetRoot= pathToTargetRoot;
      }

      RemoteFilesDeleteOperation::~RemoteFilesDeleteOperation()
      {
         if (m_toDelete != NULL) {
            delete m_toDelete;
         }
      }

      void RemoteFilesDeleteOperation::do_file_transfer_operation()
      {
         // Notify listeners that operation have started
         notifyStart();

         // Remove first file in the ::list_base
         remove(false);
      }

      void RemoteFilesDeleteOperation::onFileListReply(DataInputStream * pinput)
      {
         FileInfoList *current = m_toDelete;

         //
         // Current file is directory so, we can remove it only
         // if it contain no files, and add these files to ::list_base
         // and first remove them otherwise.
         //

         // Set child file ::list_base to current folder
         m_toDelete->setChild(m_logWriter, m_replyBuffer->m_fileinfoa);
         // Get child file ::list_base
         FileInfoList *child = m_toDelete->getChild();

         //
         // If it's not null than remove sub files.
         // Forced remove folder otherwise
         //

         if (child != NULL) {
            m_toDelete = child;
            remove(false);
         } else {
            remove(true);
         }
      }

      void RemoteFilesDeleteOperation::onRmReply(DataInputStream * pinput)
      {
         // Go to next file to delete it
         gotoNext();
      }

      void RemoteFilesDeleteOperation::onLastRequestFailedReply(DataInputStream * pinput)
      {
         //
         // Logging
         //

         ::string errorMessage;
         errorMessage = m_replyBuffer->getLastErrorMessage();

         ::string remotePath;
         remotePath = getRemotePath(m_toDelete, m_pathToTargetRoot);

         ::string scopedstrMessage;
         scopedstrMessage.formatf("Error: {} ('{}')", errorMessage,
                        remotePath);

         notifyError(scopedstrMessage);

         // Delete next file
         gotoNext();
      }

      void RemoteFilesDeleteOperation::remove(bool removeIfFolder)
      {
         if (isTerminating()) {
            killOp();
            return ;
         }

         if (m_toDelete == NULL) {
            return ;
         }

         FileInfo *fileInfo = m_toDelete->getFileInfo();

         //::string remotePath;

         auto pathRemote = getRemotePath(m_toDelete,
                       m_pathToTargetRoot);

         //
         // Logging
         //

         if (((fileInfo->isDirectory() && !removeIfFolder) ||
             (!fileInfo->isDirectory())) && (m_toDelete->getFirst()->getParent() == NULL)) {
            ::string scopedstrMessage;

            scopedstrMessage.formatf("Deleting remote '{}' {}", pathRemote,
                           fileInfo->isDirectory() ? "folder" : "file");

            notifyInformation(scopedstrMessage);
             }

         //
         // Try remove file if it's not folder or have order to forced
         // file removal.
         //

         if (!fileInfo->isDirectory() || removeIfFolder) {
            // Send request to server
            m_sender->sendRmFileRequest(pathRemote);

            //
            // If file is folder and we have order to forced removal
            // we must set folder child to NULL. If we dont do than
            // program will try to remove already removed files next step.
            //

            if ((removeIfFolder) && (fileInfo->isDirectory())) {
               m_toDelete->setChild(m_logWriter, {});
            }
         } else {
            // Send file ::list_base request cause we must remove subfolders and files from
            // folder before we can delete it
            m_sender->sendFileListRequest(pathRemote, m_replyBuffer->isCompressionSupported());
         }
      }

      void RemoteFilesDeleteOperation::gotoNext()
      {
         FileInfoList *current = m_toDelete;

         bool hasChild = current->getChild() != NULL;
         bool hasNext = current->getNext() != NULL;
         bool hasParent = current->getFirst()->getParent() != NULL;

         if (hasChild) {
            // If it has child, we must remove child file ::list_base first
            m_toDelete = current->getChild();
            remove(false);
         } else if (hasNext) {
            // If it has no child, but has next file, we must remove next file
            m_toDelete = current->getNext();
            remove(false);
         } else {

            //
            // If it has no child and not next, but has parent file,
            // we must go to parent file (folder i mean) and forced remove it
            //

            FileInfoList *first = current->getFirst();
            if (hasParent) {
               m_toDelete = first->getParent();
               remove(true);
            } else {
               //
               // If no files to delete, than clear memory and
               // operation is finished
               //
               killOp();
            } // if / else
         } // if / else
      } // void

      void RemoteFilesDeleteOperation::killOp()
      {
         delete m_toDelete->getRoot();
         m_toDelete = NULL;

         // Notify listeners that operation have ended
         notifyFinish();
      }
   }
}