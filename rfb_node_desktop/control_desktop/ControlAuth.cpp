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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "ControlAuth.h"
//#include "subsystem/platform/::string.h"
#include "remoting/rfb_node_desktop/control_desktop/ControlProto.h"

//#include "subsystem/thread/lockable_critical_section.h"

#include "subsystem/platform/DesCrypt.h"
//#include aaa_<algorithm>


namespace remoting_control_desktop
{
   ControlAuth::ControlAuth(ControlGate * pcontrolgate, const ::scoped_string & scopedstrPassword)
   : m_pcontrolgate(pcontrolgate)
   {
      // Prepare data for authentication.
      //::string truncatedPass(scopedstrPassword);
      //truncatedPass.getSubstring(&truncatedPass, 0, ::remoting_rfb_node::ServerConfig::VNC_PASSWORD_SIZE - 1);

      //::string passwordAnsi(&truncatedPass);

      memset(m_password, 0, sizeof(m_password));
      memcpy(m_password, scopedstrPassword.c_str(),
             ::minimum(scopedstrPassword.length(), sizeof(m_password)));

      // FIXME: Why it's commented out?
      // critical_section_lock l(m_pcontrolgate);

      m_pcontrolgate->writeUInt32(ControlProto::AUTH_MSG_ID);
      m_pcontrolgate->writeUInt32(0);

      authRfb();

      unsigned char result = m_pcontrolgate->readUInt32();

      switch (result) {
         case ControlProto::REPLY_ERROR:
         {
            ::string authFailReason;

            authFailReason = m_pcontrolgate->readUtf8();

            throw ::remoting_rfb_node::ControlAuthException(authFailReason);
         }
            break;
         case ControlProto::REPLY_OK:
            break;
         default:
            throw ::subsystem::Exception("Server return unknown auth result");
            break;
      }
   }

   ControlAuth::~ControlAuth()
   {
   }

   void ControlAuth::authRfb()
   {
      unsigned char challenge[16];
      unsigned char response[16];

      m_pcontrolgate->readFully(challenge, sizeof(challenge));

      ::subsystem::DesCrypt desCrypt;

      desCrypt.encrypt(response, challenge, sizeof(challenge), m_password);

      m_pcontrolgate->write(response, sizeof(response));
   }
} // namespace remoting_control_desktop

