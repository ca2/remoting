// Copyright (C) 2013 GlavSoft LLC.
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
#include "RreEncoder.h"

namespace remoting
{
   RreEncoder::RreEncoder(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream)
   : Encoder(ppixelconverter, pdataoutputstream)
   {
      m_rects.reserve(4096);
   }

   RreEncoder::~RreEncoder()
   {
   }

   int RreEncoder::getCode() const
   {
      return EncodingDefs::RRE;
   }

   void RreEncoder::splitRectangle(const ::int_rectangle &  rectangle,
                                   ::int_rectangle_array_base & rectanglea,
                                   const ::innate_subsystem::Framebuffer *serverFb,
                                   const EncodeOptions *options)
   {
      for (int y0 = rectangle.top; y0 < rectangle.bottom; y0 += RECT_SIZE) {
         for (int x0 = rectangle.left; x0 < rectangle.right; x0 += RECT_SIZE) {
            int x1 = minimum(x0 + RECT_SIZE, rectangle.right);
            int y1 = minimum(y0 + RECT_SIZE, rectangle.bottom);
            rectanglea.add(::int_rectangle(x0, y0, x1, y1));
         }
      }
   }

   void RreEncoder::sendRectangle(const ::int_rectangle &  rectangle,
                                  const ::innate_subsystem::Framebuffer *serverFb,
                                  const EncodeOptions *options)
   {
      const ::innate_subsystem::Framebuffer *pframebuffer = m_ppixelconverter->convert(rectangle, serverFb);

      size_t bpp = pframebuffer->getBitsPerPixel();
      // Choose size of pixel according to options.
      if (bpp == 8) {
         rreEncode<unsigned char>(rectangle, pframebuffer);
      } else if (bpp == 16) {
         rreEncode<unsigned short>(rectangle, pframebuffer);
      } else if (bpp == 32) {
         rreEncode<::u32>(rectangle, pframebuffer);
      } else {
         ASSERT(0);
      }
   }

   template <class PIXEL_T>
   void RreEncoder::rreEncode(const ::int_rectangle &  r,
                              const ::innate_subsystem::Framebuffer *pframebuffer)
   {
      PIXEL_T *buffer = (PIXEL_T *)pframebuffer->getBuffer();
      int fbWidth = pframebuffer->getDimension().cx;
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      // Mask for cutting rubbish bits.
      PIXEL_T mask = pixelformat.redMax << pixelformat.redShift |
                     pixelformat.greenMax << pixelformat.greenShift |
                     pixelformat.blueMax << pixelformat.blueShift;

      PIXEL_T backgroundPixelValue = buffer[r.top * fbWidth + r.left] & mask;

      // Clear the cache with m_rects.
      m_rects.resize(0);

      ::array_base<PIXEL_T> subrectPixelValue;

      // Find lines with the same pixel values.
      for (int i = r.top; i < r.bottom; i++) {
         for (int j = r.left; j < r.right; j++) {
            if ((buffer[i * fbWidth + j] & mask) != backgroundPixelValue) {
               if (subrectPixelValue.empty() ||
                   (buffer[i * fbWidth + j] & mask) != (buffer[i * fbWidth + j - 1] & mask) ||
                   m_rects.last().top != (i - r.top)) {
                  subrectPixelValue.add(buffer[i * fbWidth + j] & mask);
                  ::int_rectangle rectangle(0, 0, 1, 1);
                  rectangle.set_top_left(j - r.left, i - r.top);
                  m_rects.add(rectangle);
                   } else {
                      ++m_rects.last().right;
                   }
            }
         }
      }

      // Send header.
      m_pdataoutputstream->writeUInt32(static_cast<::u32>(subrectPixelValue.size()));
      m_pdataoutputstream->write(&backgroundPixelValue, sizeof(PIXEL_T));

      // Send subrectangles.
      for (size_t i = 0; i < subrectPixelValue.size(); i++) {
         m_pdataoutputstream->write(&subrectPixelValue[i], sizeof(PIXEL_T));
         m_pdataoutputstream->writeUInt16(m_rects[i].left);
         m_pdataoutputstream->writeUInt16(m_rects[i].top);
         m_pdataoutputstream->writeUInt16(m_rects[i].width());
         m_pdataoutputstream->writeUInt16(m_rects[i].height());
      }
   }
} // namespace  remoting
