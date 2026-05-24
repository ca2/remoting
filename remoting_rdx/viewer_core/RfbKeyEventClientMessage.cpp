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
#include "RfbKeyEventClientMessage.h"

namespace remoting_rfb_client
{
   RfbKeyEventClientMessage::RfbKeyEventClientMessage(bool downFlag, ::u32 key)
   : m_downFlag(downFlag),
     m_key(key)
   {
   }

   RfbKeyEventClientMessage::~RfbKeyEventClientMessage()
   {
   }

   void RfbKeyEventClientMessage::send(::remoting_rfb::RfbOutputGate *prfboutputgate)
   {
      critical_section_lock al(prfboutputgate);
      prfboutputgate->writeUInt8(::remoting_rfb::ClientMsgDefs::KEYBOARD_EVENT);
      prfboutputgate->writeUInt8(m_downFlag);
      prfboutputgate->writeUInt16(0); // padding
      prfboutputgate->writeUInt32(m_key);
      prfboutputgate->flush();
   }
} // namespace remoting_rfb_client