// Copyright (C) 2013 GlavSoft LLC.
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

#include "Encoder.h"
#include "remoting/remoting_rfb/region/Region.h"

namespace remoting_rfb
{


   class RreEncoder : public Encoder
   {
   public:
      RreEncoder(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream);
      virtual ~RreEncoder();

      virtual ::i32 getCode() const;

      virtual void splitRectangle(const ::i32_rectangle & rectangle, ::i32_rectangle_array_base & rectanglea,
                                  const ::innate_subsystem::Framebuffer *serverFb, const EncodeOptions *options);

      virtual void sendRectangle(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *serverFb,
                                 const EncodeOptions *options);

   private:
      template<class PIXEL_T>
      void rreEncode(const ::i32_rectangle &r, const ::innate_subsystem::Framebuffer *pframebuffer);

      // Coordinates of subrectangles.
      ::i32_rectangle_array_base m_rects;

      // All rectangles are devided (in splitRectangle() function)
      // into new rectangles with maximum size == 64.
      // ::i32_rectangle size == 64 for a better performance and less memory consumption.
      static const ::i32 RECT_SIZE = 64;
   };



} // namespace remoting_rfb



