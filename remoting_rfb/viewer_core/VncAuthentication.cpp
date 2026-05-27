// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "VncAuthentication.h"

//#include "subsystem/platform/::string.h"
#include "subsystem/platform/DesCrypt.h"
#include "subsystem/platform/VncPassCrypt.h"
//#include aaa_<algorithm>

namespace remoting_rfb_client
{


   void VncAuthentication::vncAuthenticate(::DataInputStream * pinput,
                                           ::DataOutputStream * pdataoutputstream,
                                           const ::scoped_string & password)
   {
      // TODO: removed duplicate code: ControlAuth.cpp

      // Prepare data for authentication.
      ::string truncatedPass;
      truncatedPass = password.substr( 0, ::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE);

      ::string passwordAnsi(truncatedPass);

      ::u8 m_password[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];
      memset(m_password, 0, sizeof(m_password));
      memcpy(m_password, passwordAnsi,
             ::minimum(passwordAnsi.length(), sizeof(m_password)));

      ::u8 challenge[16];
      ::u8 response[16];

      pinput->readFully(challenge, sizeof(challenge));
      
      
      ::subsystem::DesCrypt desCrypt;
      desCrypt.encrypt(response, challenge, sizeof(challenge), m_password);
      pdataoutputstream->write(response, sizeof(response));
      pdataoutputstream->flush();

   }


} // namespace remoting_rfb_client



