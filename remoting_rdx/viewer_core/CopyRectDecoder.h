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


#include "remoting/remoting_rfb/viewer_core/DecoderOfRectangle.h"

////#include "remoting/remoting_rfb/region/::i32_point.h"
namespace remoting_rfb_client
{
   class CLASS_DECL_REMOTING_RDX CopyRectDecoder : public DecoderOfRectangle
   {
   public:
      CopyRectDecoder(::subsystem::LogWriter * plogwriter);
      virtual ~CopyRectDecoder();

   protected:
      //
      // This method inherited by DecoderOfRectangle.
      //
      void decode(::remoting_rfb::RfbInputGate *input,
                          ::innate_subsystem::Framebuffer *pframebuffer,
                          const ::i32_rectangle &  rectangleTarget) override;

      //
      // This method inherited by DecoderOfRectangle.
      //
      void copy(::innate_subsystem::Framebuffer *dstFramebuffer,
                        const ::innate_subsystem::Framebuffer *pframebufferSource,
                        const ::i32_rectangle &  rectangle,
                        lockable_critical_section *pcriticalsectionFramebuffer) override;

   private:
      // This ::i32_point save left-top corner of copy-rectangle.
      ::i32_point m_sourcePosition;
   };
} // namespace remoting_rfb_client
