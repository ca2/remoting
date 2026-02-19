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
#include "FileTransferOperation.h"
#include "OperationEventListener.h"

namespace remoting
{
   namespace ftp
   {
      FileTransferOperation::FileTransferOperation(LogWriter *logWriter)
      : m_logWriter(logWriter),
        m_sender(0),
        m_replyBuffer(0),
        m_isTerminating(false)
      {
      }

      FileTransferOperation::~FileTransferOperation()
      {
      }

      void FileTransferOperation::setRequestSender(FileTransferRequestSender *sender)
      {
         m_sender = sender;
      }

      void FileTransferOperation::setReplyBuffer(FileTransferReplyBuffer *replyBuffer)
      {
         m_replyBuffer = replyBuffer;
      }

      void FileTransferOperation::terminate()
      {
         m_isTerminating = true;
      }

      bool FileTransferOperation::isTerminating()
      {
         return m_isTerminating;
      }

      void FileTransferOperation::notifyStart()
      {
         AutoLock al(&m_listeners);

         ::array_base<OperationEventListener *>::iterator it;
         for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
            OperationEventListener *l = *it;
            l->ftOpStarted(this);
         }
      }

      void FileTransferOperation::notifyFinish()
      {
         AutoLock al(&m_listeners);

         ::array_base<OperationEventListener *>::iterator it;
         for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
            OperationEventListener *l = *it;
            l->ftOpFinished(this);
         }
      }

      void FileTransferOperation::notifyError(const ::scoped_string & scopedstrMessage)
      {
         m_logWriter->debug("{}\n", ::string(scopedstrMessage).c_str());

         AutoLock al(&m_listeners);

         ::array_base<OperationEventListener *>::iterator it;
         for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
            OperationEventListener *l = *it;
            l->ftOpErrorMessage(this, scopedstrMessage);
         }
      }

      void FileTransferOperation::notifyInformation(const ::scoped_string & scopedstrMessage)
      {
         m_logWriter->debug("{}\n", scopedstrMessage);

         AutoLock al(&m_listeners);

         ::array_base<OperationEventListener *>::iterator it;
         for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
            OperationEventListener *l = *it;
            l->ftOpInfoMessage(this, scopedstrMessage);
         }

      }



      ::file::path FileTransferOperation::getLocalPath(FileInfoList *currentFile,
                                               const ::scoped_string & scopedstrLocalFolder)
      {
         ///::string pathNoRoot;
         auto path = currentFile->getAbsolutePath();
         return scopedstrLocalFolder/path;
         // out-= localFolder;
         // if (!out->endsWith(_T('\\')) && !pathNoRoot.beginsWith(_T('\\'))) {
         //   out->appendString("\\");
         // }
         // out->appendString(pathNoRoot);
      }

      ::file::path FileTransferOperation::getRemotePath(FileInfoList *currentFile,
                                                const ::scoped_string & scopedstrRemoteFolder)
      {
         //auto pathNoRoot = currentFile->getAbsolutePath(&pathNoRoot, _T('/'));

         auto pathNoRoot = currentFile->getAbsolutePath();

         // out-= remoteFolder;
         // if (!out->endsWith(_T('/')) && !pathNoRoot.beginsWith(_T('/'))) {
         //    out->appendString("/");
         // }
         // out->appendString(pathNoRoot);
         return scopedstrRemoteFolder / pathNoRoot;
      }
   }
}