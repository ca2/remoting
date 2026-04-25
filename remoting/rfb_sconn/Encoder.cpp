// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "Encoder.h"


namespace remoting
{
   Encoder::Encoder(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream)
   : m_ppixelconverter(ppixelconverter),
     m_pdataoutputstream(pdataoutputstream)
   {
   }

   Encoder::~Encoder(void)
   {
   }

   int Encoder::getCode() const
   {
      return EncodingDefs::RAW;
   }

   void Encoder::splitRectangle(const ::int_rectangle &  rect,
                                ::int_rectangle_array_base *rectList,
                                const ::innate_subsystem::FrameBuffer *serverFb,
                                const EncodeOptions *options)
   {
      rectList->add(rect);
   }

   void Encoder::sendRectangle(const ::int_rectangle &  rect,
                               const ::innate_subsystem::FrameBuffer *serverFb,
                               const EncodeOptions *options)
   {
      const ::innate_subsystem::FrameBuffer *pframebuffer = m_ppixelconverter->convert(rect, serverFb);
      int pixelSize = (int)pframebuffer->getBytesPerPixel();
      _ASSERT(pixelSize == pframebuffer->getBytesPerPixel());

      unsigned char *buffer = (unsigned char *)pframebuffer->getBuffer();
      int lineWidth = rect.width();
      int fbWidth = pframebuffer->getDimension().cx;
      int lineSizeInBytes = lineWidth * pixelSize;
      int stride = fbWidth * pixelSize;
      unsigned char *lineP = &buffer[(rect.top * fbWidth + rect.left) * pixelSize];

      // Send the rectangle as is, line by line.
      for (int i = rect.top; i < rect.bottom; i++, lineP += stride) {
         m_pdataoutputstream->write((char *)lineP, lineSizeInBytes);
      }
   }
} // namespace remoting