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

#pragma once



// #include "util/inttypes.h"
// #include "network/::remoting::RfbInputGate.h"
// #include "network/RfbOutputGate.h"
// #include "ft-common/FileInfo.h"
// #include "file-lib/WinFileChannel.h"
// #include "util/Inflater.h"
// #include "util/Deflater.h"
// #include "desktop/Desktop.h"
// #include "rfb-sconn/RfbCodeRegistrator.h"
// #include "rfb-sconn/RfbDispatcherListener.h"
// #include "FileTransferSecurity.h"
// #include "log-writer/LogWriter.h"
#include "subsystem/platform/Deflater.h"
#include "subsystem/platform/Inflater.h"
namespace remoting
{
   /**
    * Handler of file transfer plugin client to server messages.
    * Processes client requests and sends replies.
    */
   class CLASS_DECL_REMOTING FileTransferRequestHandler :
   virtual public RfbDispatcherListener
   {
   public:
      /**
       * Creates new file transfler client messages handler.
       * @param registrator rfb registrator which needs to register FT messages
       *   to rfb dispatcher address whem to this object.
       * @param output gate for writting replies for requests.
       * @param desktop windows desktop to known current user name and desktop name.
       *   File transfers are disabled if current desktop is winlogon.
       * @pararm enabled indicates if file transfer should be enabled or disabled
       *   (for example, it's disabled in view-only mode).
       */
      FileTransferRequestHandler(RfbCodeRegistrator *registrator,
                                 ::remoting::RfbOutputGate *output,
                                 Desktop *desktop,
                                 ::subsystem::LogWriter *log,
                                 bool enabled = true);

      /**
       * Deletes file transfer request handler.
       */
      virtual ~FileTransferRequestHandler();

      /**
       * Inherited from RfbDispatcherListener.
       * Processes file transfer client messages.
       */
      virtual void onRequest(unsigned int reqCode, ::remoting::RfbInputGate *backGate);

   protected:

      /**
       * Checks if file transfer if enabled.
       * @return true if file transfer is enabled, false otherwise.
       */
      bool isFileTransferEnabled();

      //
      // Common request handlers.
      //

      void compressionSupportRequested();
      void fileListRequested();
      void mkDirRequested();
      void rmFileRequested();
      void mvFileRequested();
      void dirSizeRequested();
      void md5Requested();

      //
      // Upload requests handlers.
      //

      void uploadStartRequested();
      void uploadDataRequested();
      void uploadEndRequested();

      //
      // Download requests handlers.
      //

      void downloadStartRequested();
      void downloadDataRequested();

      //
      // Method sends "Last request failed" message with error description.
      //

      void lastRequestFailed(::string & str);
      void lastRequestFailed(const char *description);

      //
      // Helper methods
      //

      bool getDirectorySize(const char *pathname, filesize *dirSize);

   //protected:
      /**
       * Checks if we can run file transfer now (using FileTransferSecurity).
       * @throws SystemException if file transfer cannot handle request now (
       * for example, winlogon desktop is active) or access denied (for example,
       * when view-only mode is enabled).
       */
      void checkAccess();// throw(SystemException);

   protected:
      //
      // Input and output gates.
      //

      ::pointer < ::remoting::RfbInputGate > m_prfbinputgate;
      ::pointer < ::remoting::RfbOutputGate > m_prfboutputgate;

      //
      // Download operation members
      //

      ::pointer < ::subsystem::FileInterface > m_pfileDownload;
      //WinFileChannel *m_fileInputStream;
      ::pointer < ::subsystem::FileInterface > m_pfileInputStream;

      //
      // Upload operation members
      //

      ::pointer < ::subsystem::FileInterface > m_pfileUpload;
      //::WinFileChannel *m_fileOutputStream;
      ::pointer < ::subsystem::FileInterface > m_pfileOutputStream;

      //
      // Zlib encoder / decoder
      //

      ::subsystem::Deflater m_deflater;
      ::subsystem::Inflater m_inflater;

      //
      // Security and impersonation.
      //

      FileTransferSecurity *m_security;

      // Determinates if file transfer is enabled.
      bool m_enabled;

      ::subsystem::LogWriter *m_plogwriter;
   };
} // namespace remoting



