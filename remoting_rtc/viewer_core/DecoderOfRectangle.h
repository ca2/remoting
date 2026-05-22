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

#pragma once


#include "Decoder.h"


namespace remoting_client
{
   class FbUpdateNotifier;

   class CLASS_DECL_REMOTING DecoderOfRectangle : public Decoder
   {
   public:
      DecoderOfRectangle(::subsystem::LogWriter * plogwriter);
      virtual ~DecoderOfRectangle();

      //
      // This function does the following:
      //   1. read update of rectangleTarget from input
      //   2. decode rectangle on "secondFramebuffer"
      //   3. copy rectangle from secondFramebuffer to "pframebuffer"
      //   4. notify fbNotifier
      // His called decode(), copy() and notify() by order, defined in implementation.
      //
      // This function is thread-safe for pframebuffer->
      //
      virtual void process(::remoting_rtc::RfbInputGate *input,
                           ::innate_subsystem::Framebuffer *pframebuffer,
                           ::innate_subsystem::Framebuffer *secondFramebuffer,
                           const ::i32_rectangle &  rectangle,
                           lockable_critical_section *pcriticalsectionFramebuffer,
                           FbUpdateNotifier *fbNotifier);

      //
      // This method inherited Decoder::isPseudo() and return true.
      //
      virtual bool isPseudo() const;

   protected:
      //
      // This method read rectangle-update from input and decode on pframebuffer->
      //
      virtual void decode(::remoting_rtc::RfbInputGate *input,
                          ::innate_subsystem::Framebuffer *pframebuffer,
                          const ::i32_rectangle &  rectangle) = 0;

      //
      // This method copy rectangle from pframebufferSource to dstFramebuffer.
      // This function is thread-safe from dstFramebuffer.
      //
      virtual void copy(::innate_subsystem::Framebuffer *dstFramebuffer,
                        const ::innate_subsystem::Framebuffer *pframebufferSource,
                        const ::i32_rectangle &  rectangle,
                        lockable_critical_section *pcriticalsectionFramebuffer);

      //
      // This method notify fbNotifier about update of rectangle.
      //
      virtual void notify(FbUpdateNotifier *fbNotifier,
                          const ::i32_rectangle &  rectangle);
   };
} // namespace remoting_client

