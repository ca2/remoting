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
#include "RreDecoder.h"


namespace remoting_client
{
   RreDecoder::RreDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter)
   {
      m_encoding = ::remoting_rtc::EncodingDefs::RRE;
   }

   RreDecoder::~RreDecoder()
   {
   }

   void RreDecoder::decode(::remoting_rtc::RfbInputGate *pinput,
                           ::innate_subsystem::Framebuffer *pframebuffer,
                           const ::i32_rectangle &  rectangleTarget)
   {
      ::u32 numberRectangle = pinput->readUInt32();
      size_t bytesPerPixel = pframebuffer->getBytesPerPixel();

      ::u32 backgroundColor;
      pinput->readFully(&backgroundColor, bytesPerPixel);
      pframebuffer->fillRect(rectangleTarget, backgroundColor);

      while (numberRectangle--) {
         ::u32 color;
         pinput->readFully(&color, bytesPerPixel);
         ::u32 x = pinput->readUInt16();
         ::u32 y = pinput->readUInt16();
         ::u32 w = pinput->readUInt16();
         ::u32 h = pinput->readUInt16();

         ::i32_rectangle rectangle(x, y, x + w, y + h);
         rectangle.offset(rectangleTarget.left, rectangleTarget.top);
         pframebuffer->fillRect(rectangle, color);
      }
   }
} // namespace remoting_client