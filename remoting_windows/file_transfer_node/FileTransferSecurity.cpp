// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the T i g h t V N C software.  Please visit our Web site:
//
//                       http://www.t i g h t v n c.com/
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
#include "FileTransferSecurity.h"

#include "remoting/remoting/node_config/Configurator.h"
#include "subsystem_windows/node/WTS.h"
#include "subsystem_windows/platform/subsystem.h"


namespace remoting
{


   namespace file_transfer
   {
      FileTransferSecurity::FileTransferSecurity(::remoting_node::Configurator * pconfigurator, Desktop *desktop, ::subsystem::LogWriter *plogwriter)
      :
      //Impersonator(plogwriter),
        m_hasAccess(false),
        m_pdesktop(desktop),
        m_plogwriter(plogwriter),
      m_pconfigurator(pconfigurator)
      {
         m_pdesktop = desktop;
      }

      FileTransferSecurity::~FileTransferSecurity()
      {
      }

      void FileTransferSecurity::beginMessageProcessing()
      {
         ::remoting_node::Configurator* conf = m_pconfigurator;
         bool runAsService = conf->getServiceFlag();
         bool rdpEnabled = conf->getServerConfig()->getConnectToRdpFlag();

         if (!runAsService) {
            m_hasAccess = true;
            return;
         }
         try {
            ::string userName, desktopName;

            if (m_pdesktop != NULL) {
               m_pdesktop->getCurrentUserInfo(desktopName, userName);
            }

            desktopName.make_lower();

            // FIXME: Why we compare desktop name? why only default desktop?
            if (desktopName != "default") {
               throw ::subsystem::Exception("Desktop is not default desktop.");
            }

            if (sessionIsLocked(rdpEnabled)) {
               throw ::subsystem::Exception("Desktop is locked.");
            }

            if (rdpEnabled && (WindowsSubsystem().WTS().getRdpSessionId(m_plogwriter) != 0)) {
               HANDLE token = WindowsSubsystem().WTS().duplicateCurrentProcessUserToken(rdpEnabled, m_plogwriter);
               impersonateAsUser(token);
            }
            else {
               impersonateAsLoggedUser();
            }


            m_hasAccess = true;
         } catch (::subsystem::Exception &e) {
            m_plogwriter->error("Access denied to the file transfer: {}",
                         e.get_message());
            m_hasAccess = false;
         } // try / catch.
      }

      void FileTransferSecurity::throwIfAccessDenied()
      {
         if (!m_hasAccess) {
            throw ::subsystem::Exception("Access denied.");
         } else if (!m_pconfigurator->getServerConfig()->isFileTransfersEnabled()) {
            throw ::subsystem::Exception("File transfers are disabled on server side.");
         }
      }

      void FileTransferSecurity::endMessageProcessing()
      {
         if (m_pconfigurator->getServiceFlag() && m_hasAccess) {
            try {
               revertToSelf();
            } catch (...) {
            } // try / catch.
         } // if onThreadMain as service.
      }


   } // namespace file_transfer


} // namespace remoting