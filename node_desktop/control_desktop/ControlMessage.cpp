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
#include "ControlMessage.h"
#include "ControlAuth.h"
#include "ControlAuthDialog.h"
#include "ControlProto.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/config/RegistrySettingsManager.h"
#include "subsystem/platform/VncPassCrypt.h"
//#include "subsystem/platform/::string.h"
#include "remoting/remoting/node/NamingDefs.h"
//#include "file_lib/WinFile.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "remoting/node_desktop/resource.h"
#include "subsystem/platform/Registry.h"
#include "remoting/remoting/platform/remoting.h"

//#include aaa_<crtdbg.h>

namespace remoting_control_desktop
{


   ControlMessage::ControlMessage(::u32 messageId, ControlGate * pcontrolgate,
                                  const ::scoped_string & scopedstrPasswordFile,
                                  bool getPassFromConfigEnabled,
                                  bool forService)
   : DataOutputStream(0), m_messageId(messageId), m_pcontrolgate(pcontrolgate),
     m_passwordFile(scopedstrPasswordFile),
     m_getPassFromConfigEnabled(getPassFromConfigEnabled),
     m_forService(forService)
   {
      m_tunnel = new ByteArrayOutputStream(this,2048);

      m_poutputstream = m_tunnel;
   }

   ControlMessage::~ControlMessage()
   {
      delete m_tunnel;
   }

   void ControlMessage::send()
   {
      sendData();

      checkRetCode();
   }

   void ControlMessage::sendData()
   {
      m_pcontrolgate->writeUInt32(m_messageId);
      ASSERT((::u32)m_tunnel->size() == m_tunnel->size());
      m_pcontrolgate->writeUInt32((::u32)m_tunnel->size());
      m_pcontrolgate->write(m_tunnel->toByteArray(), m_tunnel->size());
   }

   void ControlMessage::checkRetCode()
   {
      ::u32 messageId = m_pcontrolgate->readUInt32();

      switch (messageId) {
         case ControlProto::REPLY_ERROR:
         {
            ::string strMessage;
            strMessage = m_pcontrolgate->readUtf8();
            throw ::remoting_node::RemoteException(strMessage);
         }
            break;
         case ControlProto::REPLY_AUTH_NEEDED:
            if (m_passwordFile.length() != 0) {
               authFromFile();
            } else if (m_getPassFromConfigEnabled) {
               authFromRegistry();
            } else {
               ControlAuthDialog authDialog;

               int retCode = authDialog.showModal();
               switch (retCode) {
                  case ::innate_subsystem::e_control_id_cancel:
                     throw ::remoting_node::ControlAuthException(MainSubsystem().StringTable().getString(IDS_USER_CANCEL_CONTROL_AUTH), true);
                  case ::innate_subsystem::e_control_id_ok:
                     ControlAuth auth(m_pcontrolgate, authDialog.getPassword());
                     send();
                     break;
               }
            }
            break;
         case ControlProto::REPLY_OK:
            break;
         default:
            ASSERT(false);
            throw ::remoting_node::RemoteException("Unknown ret code.");
      }
   }

   void ControlMessage::authFromFile()
   {

      auto password = file()->as_string(m_passwordFile);
      password.truncate(8);
      auto pfind =password.find('\r');
      if (pfind) password.truncate(pfind);
      pfind =password.find('\n');
      if (pfind) password.truncate(pfind);
      // WinFile file(m_passwordFile, F_READ, FM_OPEN);
      // char ansiBuff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      // file.read(ansiBuff, 8);
      // for (int i = 0; i < 8; i++) {
      //    if (ansiBuff[i] == '\r' || ansiBuff[i] == '\n') {
      //       ansiBuff[i] = '\0';
      //    }
      // }
      // ::string ansiPwd(ansiBuff);
      // ::string password;
      // ansiPwd.toStringStorage(&password);
      ControlAuth auth(m_pcontrolgate, password);
      send();
   }

   void ControlMessage::authFromRegistry()
   {
      auto rootKey = m_forService ? MainSubsystem().Registry().getLocalMachineKey() : MainSubsystem().Registry().getCurrentUserKey();
      ::remoting::RegistrySettingsManager sm(rootKey, ::remoting_node::RegistryPaths::SERVER_PATH, 0);

      unsigned char hidePassword[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      unsigned char plainPassword[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      memsize passSize = sizeof(hidePassword);

      if (sm.getBinaryData("ControlPassword",
                           hidePassword,
                           &passSize)) {
         ::subsystem::VncPassCrypt::getPlainPass(plainPassword, hidePassword);

         ///::string plainAnsiString((char *)plainPassword);
         ::string password;
         //plainAnsiString.toStringStorage(&password);
         password = ::str::to_ansi(plainPassword, sizeof(plainPassword));
         // Clear ansi plain password from memory.
         memset(plainPassword, 0, sizeof(plainPassword));
         ControlAuth auth(m_pcontrolgate, password);

         send();
                           } else {
                              // Ignore errors for silent.
                           }
   }
} // namespace remoting_control_desktop

