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

#pragma once


#include "DecoderOfRectangle.h"

namespace remoting_rfb_client
{
   class CLASS_DECL_REMOTING_RDX RawDecoder : public DecoderOfRectangle
   {
   public:
      RawDecoder(::subsystem::LogWriter * plogwriter);
      virtual ~RawDecoder();

      //
      // This method is inherited from DecoderOfRectangle.
      //
      virtual void process(::remoting_rfb::RfbInputGate *input,
                           ::innate_subsystem::Framebuffer *pframebuffer,
                           ::innate_subsystem::Framebuffer *secondFramebuffer,
                           const ::i32_rectangle &  rectangle,
                           lockable_critical_section *pcriticalsectionFramebuffer,
                           FbUpdateNotifier *fbNotifier);

   protected:
      virtual void decode(::remoting_rfb::RfbInputGate *input,
                          ::innate_subsystem::Framebuffer *pframebuffer,
                          const ::i32_rectangle &  rectangle);

   private:
      static const size_t AREA_OF_ONE_PART = 1024 * 64;
   };
} // namespace remoting_rfb_client