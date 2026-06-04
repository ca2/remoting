// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#include "TightEncoder.h"

namespace remoting_rfb
{


   class JpegEncoder : public Encoder
   {
   public:
      // JpegEncoder is constructed as a wrapper for an existing TightEncoder
      // object. It will use that TightEncoder to perform actual compression.
      JpegEncoder(TightEncoder *tightEncoder);
      virtual ~JpegEncoder();

      // Overloaded function calls TightEncoder::getCode().
      virtual ::i32 getCode() const;

      // JpegEncoder implements its own splitRectangle() which just makes sure all
      // rectangles are no wider than 2048 pixels.
      virtual void splitRectangle(const ::i32_rectangle & rectangle, ::i32_rectangle_array_base & rectanglea,
                                  const ::innate_subsystem::Framebuffer *serverFb, const EncodeOptions *options);

      // Encode and send the rectangle. JpegEncoder forces JPEG sub-encoding if
      // JPEG quality level was set by the client and both client and server pixel
      // formats are at least 16 bits per pixel. Otherwise, it falls back to usual
      // lossless Tight encoding.
      virtual void sendRectangle(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *serverFb,
                                 const EncodeOptions *options);

   protected:
      TightEncoder *m_tightEncoder;
   };

 

} // namespace remoting_rfb
 