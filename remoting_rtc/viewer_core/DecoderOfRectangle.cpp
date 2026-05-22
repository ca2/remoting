// Copyright (C) 2012 GlavSoft LLC.
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
#include "DecoderOfRectangle.h"

#include "FbUpdateNotifier.h"

namespace remoting_client
{
   DecoderOfRectangle::DecoderOfRectangle(::subsystem::LogWriter * plogwriter)
   : Decoder(plogwriter)
   {
   }

   DecoderOfRectangle::~DecoderOfRectangle()
   {
   }

   void DecoderOfRectangle::process(::remoting_rtc::RfbInputGate *input,
                        ::innate_subsystem::Framebuffer *pframebuffer,
                        ::innate_subsystem::Framebuffer *secondFramebuffer,
                        const ::i32_rectangle &  rectangle,
                        lockable_critical_section *pcriticalsectionFramebuffer,
                        FbUpdateNotifier *fbNotifier)
   {
      decode(input, secondFramebuffer, rectangle);
      copy(pframebuffer, secondFramebuffer, rectangle, pcriticalsectionFramebuffer);
      notify(fbNotifier, rectangle);
   }

   void DecoderOfRectangle::copy(::innate_subsystem::Framebuffer *dstFramebuffer,
                      const ::innate_subsystem::Framebuffer *pframebufferSource,
                      const ::i32_rectangle &  rectangle,
                      lockable_critical_section *pcriticalsectionFramebuffer)
   {
      critical_section_lock al(pcriticalsectionFramebuffer);
      dstFramebuffer->copyFrom(rectangle, pframebufferSource, rectangle.left, rectangle.top);
   }

   void DecoderOfRectangle::notify(FbUpdateNotifier *fbNotifier,
                        const ::i32_rectangle &  rectangle)
   {
      fbNotifier->onUpdate(rectangle);
   }

   bool DecoderOfRectangle::isPseudo() const
   {
      return false;
   }
} // namespace remoting_client