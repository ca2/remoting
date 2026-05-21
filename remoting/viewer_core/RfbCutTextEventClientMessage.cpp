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
//#include "subsystem/platform/::string.h"
////#include "subsystem/platform/::string.h"


#include "RfbCutTextEventClientMessage.h"

namespace remoting_client
{
   RfbCutTextEventClientMessage::RfbCutTextEventClientMessage(const ::scoped_string & cutText)
   : m_cutText(cutText)
   {
   }

   RfbCutTextEventClientMessage::~RfbCutTextEventClientMessage()
   {
   }

   void RfbCutTextEventClientMessage::send(::remoting::RfbOutputGate *prfboutputgate)
   {
      ::string cutTextAnsi;
      cutTextAnsi = m_cutText;
      ::u32 length = static_cast<::u32>(cutTextAnsi.length());

      critical_section_lock al(prfboutputgate);
      prfboutputgate->writeUInt8(::remoting::ClientMsgDefs::CLIENT_CUT_TEXT);
      prfboutputgate->writeUInt8(0); // padding 3 bytes
      prfboutputgate->writeUInt8(0);
      prfboutputgate->writeUInt8(0);
      prfboutputgate->writeUInt32(length);
      prfboutputgate->write(cutTextAnsi, length);
      prfboutputgate->flush();
   }

   void RfbCutTextEventClientMessage::sendUtf8(::remoting::RfbOutputGate *prfboutputgate)
   {
      ::string cutTextUtf;
      cutTextUtf = m_cutText;
      ::u32 length = static_cast<::u32>(cutTextUtf.length());

      critical_section_lock al(prfboutputgate);
      prfboutputgate->writeUInt32(::remoting::ClientMsgDefs::CLIENT_CUT_TEXT_UTF8);
      prfboutputgate->writeUInt32(length);
      prfboutputgate->write(cutTextUtf, length);
      prfboutputgate->flush();
   }
} // namespace remoting_client