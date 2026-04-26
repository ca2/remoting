// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "RawDecoder.h"
#include "subsystem/platform/Exception.h"

//#include aaa_<algorithm>

namespace remoting
{
   RawDecoder::RawDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter)
   {
      m_encoding = EncodingDefs::RAW;
   }

   RawDecoder::~RawDecoder()
   {
   }

   void RawDecoder::process(RfbInputGate *input,
                            ::innate_subsystem::Framebuffer *pframebuffer,
                            ::innate_subsystem::Framebuffer *secondFramebuffer,
                            const ::int_rectangle &  rectangle,
                            critical_section *fbLock,
                            FbUpdateNotifier *fbNotifier)
   {
      // If area of rectangle is 0, then exit from process: nothing update.
      if (rectangle.area() == 0) {
         return;
      }

      int width = rectangle.width();
      int height = rectangle.height();

      // Division with round to up.
      int deltaHeight = (AREA_OF_ONE_PART + width - 1) / width;

      ::int_rectangle deltaRect(0, 0, width, deltaHeight);
      deltaRect.set_top_left(rectangle.left, rectangle.top);

      // Process all rectangle without last part of rectangle or
      // two last part, if area of last part is less half of AREA_OF_ONE_PART.
      while (deltaRect.bottom + deltaHeight / 2 < rectangle.bottom) {
         DecoderOfRectangle::process(input,
                                     pframebuffer, secondFramebuffer, deltaRect, fbLock,
                                     fbNotifier);

         // Increment pointPosition of rectangle.
         deltaRect.offset(0, deltaHeight);
      }

      // And process remainder parts of rectangle.
      deltaRect.top = ::maximum(rectangle.top, deltaRect.bottom - deltaHeight);
      deltaRect.bottom = rectangle.bottom;
      DecoderOfRectangle::process(input,
                                  pframebuffer, secondFramebuffer, deltaRect, fbLock,
                                  fbNotifier);
   }

   void RawDecoder::decode(RfbInputGate *pinput,
                        ::innate_subsystem::Framebuffer *pframebuffer,
                        const ::int_rectangle &  rectangle)
   {
      size_t bytesPerPixel = pframebuffer->getPixelFormat().bitsPerPixel / 8;
      size_t bytesPerLine = bytesPerPixel * rectangle.width();

      if (::int_rectangle(pframebuffer->getDimension()).intersection(rectangle) != rectangle)
         throw ::subsystem::Exception("Error in protocol: incorrect size of rectangle");
      for (int y = rectangle.top; y < rectangle.bottom; y++)
         pinput->readFully(pframebuffer->getBufferPtr(rectangle.left, y), bytesPerLine);
   }
} // namespace remoting