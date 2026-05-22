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
#include "RfbSetEncodingsClientMessage.h"
#include "DecoderStore.h"

namespace remoting_rfb_client
{
   RfbSetEncodingsClientMessage::RfbSetEncodingsClientMessage(const ::array_base<int> & encodings)
   {
      m_encodings = encodings;
   }

   RfbSetEncodingsClientMessage::~RfbSetEncodingsClientMessage()
   {
   }

   void RfbSetEncodingsClientMessage::send(::remoting_rfb::RfbOutputGate *prfboutputgate)
   {
      critical_section_lock al(prfboutputgate);
      prfboutputgate->writeUInt8(::remoting_rfb::ClientMsgDefs::SET_ENCODINGS);
      prfboutputgate->writeUInt8(0); // padding 1 byte

      // prfboutputgate count of encoding and out code of all encodings
      prfboutputgate->writeUInt16(static_cast<unsigned short>(m_encodings.size()));

      for (::array_base<int>::iterator i = m_encodings.begin();
           i != m_encodings.end();
           i++) {
         prfboutputgate->writeInt32(*i);
           }

      prfboutputgate->flush();
   }
} // namespace remoting_rfb_client