// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "PixelConverter.h"
//#include "subsystem/platform/inttypes.h"
////#include aaa_<crtdbg.h>
///
namespace remoting_rfb
{
   PixelConverter::PixelConverter(void)
   : m_convertMode(NO_CONVERT),
     m_dstFramebuffer(0)
   {
   }

   PixelConverter::~PixelConverter(void)
   {
      reset();
   }

   void PixelConverter::convert(const ::i32_rectangle &  rectangle, ::innate_subsystem::Framebuffer *pframebufferTarget,
                                const ::innate_subsystem::Framebuffer *pframebufferSource) const
   {
      if (m_convertMode == NO_CONVERT) {
         pframebufferTarget->copyFrom(rectangle, pframebufferSource, rectangle.left, rectangle.top);
      } else {
         ::i32 rectHeight = rectangle.height();
         ::i32 rectWidth = rectangle.width();
         ::i32 fbWidth = pframebufferTarget->getDimension().cx;
         ::innate_subsystem::PixelFormat pixelformatTarget = pframebufferTarget->getPixelFormat();
         ::innate_subsystem::PixelFormat pixelformatSource = pframebufferSource->getPixelFormat();

         ::u32 dstPixelSize = pixelformatTarget.bitsPerPixel / 8;
         ::u32 srcPixelSize = pixelformatSource.bitsPerPixel / 8;

         // FIXME: Make ::innate_subsystem::Framebuffer do the math.
         ::u8 *dstPixP = (::u8 *)pframebufferTarget->getBuffer() +
                          (fbWidth * rectangle.top + rectangle.left) * dstPixelSize;
         ::u8 *srcPixP = (::u8 *)pframebufferSource->getBuffer() +
                          (fbWidth * rectangle.top + rectangle.left) * srcPixelSize;
         if (m_convertMode == CONVERT_FROM_16) {
            for (::i32 i = 0; i < rectHeight; i++,
                 dstPixP += (fbWidth - rectWidth) * dstPixelSize,
                 srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
               for (::i32 j = 0; j < rectWidth; j++,
                                              dstPixP += dstPixelSize,
                                              srcPixP += srcPixelSize) {
                  ::u32 dstPixel = m_hexBitsTable[*(::u16 *)srcPixP];
                  if (dstPixelSize == 4) {
                     *(::u32 *)dstPixP = (::u32)dstPixel;
                  } else if (dstPixelSize == 2) {
                     *(::u16 *)dstPixP = (::u16)dstPixel;
                  } else if (dstPixelSize == 1) {
                     *(::u8 *)dstPixP = (::u8)dstPixel;
                  }
                                              }
                 }
         } else if (m_convertMode == CONVERT_FROM_32) {
            bool bigEndianDiffs = pixelformatTarget.bigEndian != pixelformatSource.bigEndian;
            ::u32 srcRedMax = pixelformatSource.redMax;
            ::u32 srcGrnMax = pixelformatSource.greenMax;
            ::u32 srcBluMax = pixelformatSource.blueMax;

            for (::i32 i = 0; i < rectHeight; i++,
                 dstPixP += (fbWidth - rectWidth) * dstPixelSize,
                 srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
               for (::i32 j = 0; j < rectWidth; j++,
                                              dstPixP += dstPixelSize,
                                              srcPixP += srcPixelSize) {
                  ::u32 dstPixel = m_redTable[*(::u32 *)srcPixP >>
                                               pixelformatSource.redShift & srcRedMax] |
                                    m_grnTable[*(::u32 *)srcPixP >>
                                               pixelformatSource.greenShift & srcGrnMax] |
                                    m_bluTable[*(::u32 *)srcPixP >>
                                               pixelformatSource.blueShift & srcBluMax];
                  if (dstPixelSize == 4) {
                     *(::u32 *)dstPixP = dstPixel;
                     if (bigEndianDiffs) {
                        *(::u32 *)dstPixP = rotateUint32(*(::u32 *)dstPixP);
                     }
                  } else if (dstPixelSize == 2) {
                     *(::u16 *)dstPixP = dstPixel;
                     if (bigEndianDiffs) {
                        *(::u16 *)dstPixP = *(::u16 *)dstPixP << 8 |
                                             *(::u16 *)dstPixP >> 8;
                     }
                  } else if (dstPixelSize == 1) {
                     *(::u8 *)dstPixP = dstPixel;
                  }
                                              }
                 }
         } else {
            ASSERT(0);
         }
      }
   }

   const ::innate_subsystem::Framebuffer *
   PixelConverter::convert(const ::i32_rectangle &  rectangle, const ::innate_subsystem::Framebuffer *pframebufferSource)
   {
      if (m_convertMode == NO_CONVERT) {
         return pframebufferSource;
      }

      const ::i32_size fbSize = pframebufferSource->getDimension();
      if (m_dstFramebuffer == 0) {
         // No frame buffer allocated - construct new one from the scratch.
         m_dstFramebuffer = new ::innate_subsystem::Framebuffer;
         m_dstFramebuffer->setProperties(fbSize, m_dstFormat);
      } else if (m_dstFramebuffer->getDimension() != fbSize)
      {
         // Frame buffer is allocated but its size it wrong - just resize it.
         // Note that if the frame buffer is allocated, its pixel format is
         // guaranteed to be relevant, because setPixelFormats() always calls
         // reset() if at least one pixel format has been changed.
         m_dstFramebuffer->setDimension(fbSize);
      }

      // Finally, convert pixels.
      convert(rectangle, m_dstFramebuffer, pframebufferSource);
      return m_dstFramebuffer;
   }

   void PixelConverter::reset()
   {
      // Deallocate the pframebuffer->
      if (m_dstFramebuffer != 0) {
         delete m_dstFramebuffer;
         m_dstFramebuffer = 0;
      }
   }

   void PixelConverter::setPixelFormats(const ::innate_subsystem::PixelFormat & pixelformatTarget,
                                        const ::innate_subsystem::PixelFormat & pixelformatSource)
   {
      if (pixelformatSource != m_srcFormat || pixelformatTarget != m_dstFormat) {
         // Reset both translation tables and the internal frame buffer.
         reset();

         if (pixelformatSource == pixelformatTarget) {
            m_convertMode = NO_CONVERT;
         } else if (pixelformatSource.bitsPerPixel == 16) { // 16 bit -> N
            m_convertMode = CONVERT_FROM_16;
            fillHexBitsTable(pixelformatTarget, pixelformatSource);
         } else if (pixelformatSource.bitsPerPixel == 32) { // 32 bit -> N
            m_convertMode = CONVERT_FROM_32;
            fill32BitsTable(pixelformatTarget, pixelformatSource);
         }

         m_srcFormat = pixelformatSource;
         m_dstFormat = pixelformatTarget;
      }
   }

   size_t PixelConverter::getSrcBitsPerPixel() const
   {
      return m_srcFormat.bitsPerPixel;
   }

   size_t PixelConverter::getDstBitsPerPixel() const
   {
      return m_dstFormat.bitsPerPixel;
   }

   void PixelConverter::fillHexBitsTable(const ::innate_subsystem::PixelFormat & pixelformatTarget,
                                         const ::innate_subsystem::PixelFormat & pixelformatSource)
   {
      m_hexBitsTable.resize(65536);

      ::u32 dstRedMax = pixelformatTarget.redMax;
      ::u32 dstGrnMax = pixelformatTarget.greenMax;
      ::u32 dstBluMax = pixelformatTarget.blueMax;

      ::u32 dstRedShift = pixelformatTarget.redShift;
      ::u32 dstGrnShift = pixelformatTarget.greenShift;
      ::u32 dstBluShift = pixelformatTarget.blueShift;

      ::u32 srcRedMax = pixelformatSource.redMax;
      ::u32 srcGrnMax = pixelformatSource.greenMax;
      ::u32 srcBluMax = pixelformatSource.blueMax;

      ::u32 srcRedMask = srcRedMax << pixelformatSource.redShift;
      ::u32 srcGrnMask = srcGrnMax << pixelformatSource.greenShift;
      ::u32 srcBluMask = srcBluMax << pixelformatSource.blueShift;

      for (::u32 i = 0; i < 65536; i++) {
         // Get source color component
         ::u32 srcRed = (i & srcRedMask) >> pixelformatSource.redShift;
         ::u32 srcGrn = (i & srcGrnMask) >> pixelformatSource.greenShift;
         ::u32 srcBlu = (i & srcBluMask) >> pixelformatSource.blueShift;

         ::u32 dstRed = (srcRed * dstRedMax / srcRedMax) << dstRedShift;
         ::u32 dstGrn = (srcGrn * dstGrnMax / srcGrnMax) << dstGrnShift;
         ::u32 dstBlu = (srcBlu * dstBluMax / srcBluMax) << dstBluShift;
         m_hexBitsTable[i] = dstRed | dstGrn | dstBlu;
         if (pixelformatTarget.bigEndian != pixelformatSource.bigEndian) {
            if (pixelformatTarget.bitsPerPixel == 32) {
               m_hexBitsTable[i] = rotateUint32(m_hexBitsTable[i]);
            }
            else if (pixelformatTarget.bitsPerPixel == 16) {
               m_hexBitsTable[i] = (m_hexBitsTable[i] & 0xff) << 8 | (m_hexBitsTable[i] & 0xff00) >> 8;
            }
         }
      }
   }

   void PixelConverter::fill32BitsTable(const ::innate_subsystem::PixelFormat & pixelformatTarget,
                                        const ::innate_subsystem::PixelFormat & pixelformatSource)
   {
      ::u32 dstRedMax = pixelformatTarget.redMax;
      ::u32 dstGrnMax = pixelformatTarget.greenMax;
      ::u32 dstBluMax = pixelformatTarget.blueMax;

      ::u32 dstRedShift = pixelformatTarget.redShift;
      ::u32 dstGrnShift = pixelformatTarget.greenShift;
      ::u32 dstBluShift = pixelformatTarget.blueShift;

      ::u32 srcRedMax = pixelformatSource.redMax;
      ::u32 srcGrnMax = pixelformatSource.greenMax;
      ::u32 srcBluMax = pixelformatSource.blueMax;

      m_redTable.resize(srcRedMax + 1);
      m_grnTable.resize(srcGrnMax + 1);
      m_bluTable.resize(srcBluMax + 1);

      for (::u32 i = 0; i <= srcRedMax; i++) {
         m_redTable[i] = ((i * dstRedMax + srcRedMax / 2) / srcRedMax) << dstRedShift;
      }
      for (::u32 i = 0; i <= srcGrnMax; i++) {
         m_grnTable[i] = ((i * dstGrnMax + srcGrnMax / 2) / srcGrnMax) << dstGrnShift;
      }
      for (::u32 i = 0; i <= srcBluMax; i++) {
         m_bluTable[i] = ((i * dstBluMax + srcBluMax / 2) / srcBluMax) << dstBluShift;
      }
   }

   ::u32 PixelConverter::rotateUint32(::u32 value) const
   {
      ::u32 result;
      char_pointer src = (char_pointer )&value;
      char_pointer dst = (char_pointer )&result;
      dst[0] = src[3];
      dst[1] = src[2];
      dst[2] = src[1];
      dst[3] = src[0];

      return result;
   }
} // namespace remoting_rfb
