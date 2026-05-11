// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "JpegEncoder.h"

namespace remoting
{
   JpegEncoder::JpegEncoder(TightEncoder *tightEncoder)
   : Encoder(tightEncoder->m_ppixelconverter, tightEncoder->m_pdataoutputstream),
     m_tightEncoder(tightEncoder)
   {
   }

   JpegEncoder::~JpegEncoder()
   {
   }

   int JpegEncoder::getCode() const
   {
      return m_tightEncoder->getCode();
   }

   void JpegEncoder::splitRectangle(const ::i32_rectangle &  rectangle,
                                    ::int_rectangle_array_base & rectanglea,
                                    const ::innate_subsystem::Framebuffer *serverFb,
                                    const EncodeOptions *options)
   {
      int maxWidth = 2048;
      for (int x0 = rectangle.left; x0 < rectangle.right; x0 += maxWidth) {
         int x1 = (x0 + maxWidth <= rectangle.right) ? x0 + maxWidth : rectangle.right;
         rectanglea.add(::i32_rectangle(x0, rectangle.top, x1, rectangle.bottom));
      }
   }

   void JpegEncoder::sendRectangle(const ::i32_rectangle &  rectangle,
                                   const ::innate_subsystem::Framebuffer *serverFb,
                                   const EncodeOptions *options)
   {
      size_t bppServer = m_ppixelconverter->getSrcBitsPerPixel();
      size_t bppClient = m_ppixelconverter->getDstBitsPerPixel();
      bool goodColorResolution = (bppServer >= 16 && bppClient >= 16);

      if (options->jpegEnabled() && goodColorResolution) {
         m_tightEncoder->sendJpegRect(rectangle, serverFb, options);
      } else {
         m_tightEncoder->sendRectangle(rectangle, serverFb, options);
      }
   }
} // namespace remoting