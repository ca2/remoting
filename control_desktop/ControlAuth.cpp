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
#include "ControlAuth.h"
//#include "subsystem/platform/::string.h"
#include "remoting/control_desktop/ControlProto.h"

//#include "subsystem/thread/critical_section.h"

#include "subsystem/platform/DesCrypt.h"
//#include aaa_<algorithm>

ControlAuth::ControlAuth(ControlGate *pblockinggate, const ::scoped_string & scopedstrPassword)
: m_pblockinggate(pblockinggate)
{
  // Prepare data for authentication.
  ::string truncatedPass(password);
  truncatedPass.getSubstring(&truncatedPass, 0, ServerConfig::VNC_PASSWORD_SIZE - 1);

  ::string passwordAnsi(&truncatedPass);

  memset(m_password, 0, sizeof(m_password));
  memcpy(m_password, passwordAnsi,
         ::minimum(passwordAnsi.length(), sizeof(m_password)));

  // FIXME: Why it's commented out?
  // critical_section_lock l(m_pblockinggate);

  m_pblockinggate->writeUInt32(ControlProto::AUTH_MSG_ID);
  m_pblockinggate->writeUInt32(0);

  authRfb();

  unsigned char result = m_pblockinggate->readUInt32();

  switch (result) {
  case ControlProto::REPLY_ERROR:
    {
      ::string authFailReason;

      m_pblockinggate->readUTF8(&authFailReason);

      throw ControlAuthException(authFailReason);
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

  m_pblockinggate->readFully(challenge, sizeof(challenge));

  DesCrypt desCrypt;

  desCrypt.encrypt(response, challenge, sizeof(challenge), m_password);

  m_pblockinggate->writeFully(response, sizeof(response));
}
