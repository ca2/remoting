// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "CopyRectDecoder.h"


namespace remoting_client
{
   CopyRectDecoder::CopyRectDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter)
   {
      m_encoding = ::remoting::EncodingDefs::COPYRECT;
   }

   CopyRectDecoder::~CopyRectDecoder()
   {
   }

   void CopyRectDecoder::decode(::remoting::RfbInputGate *pinput,
                                ::innate_subsystem::Framebuffer *pframebuffer,
                                const ::i32_rectangle &  rectangleTarget)
   {
      m_sourcePosition.x = pinput->readInt16();
      m_sourcePosition.y = pinput->readInt16();
   }

   void CopyRectDecoder::copy(::innate_subsystem::Framebuffer *dstFramebuffer,
                              const ::innate_subsystem::Framebuffer *pframebufferSource,
                              const ::i32_rectangle &  rectangle,
                              lockable_critical_section *pcriticalsectionFramebuffer)
   {
      critical_section_lock al(pcriticalsectionFramebuffer);
      dstFramebuffer->move(rectangle, m_sourcePosition.x, m_sourcePosition.y);
   }
} // namespace remoting_client