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
#include "PixelConverter.h"
#include "util/inttypes.h"
#include <crtdbg.h>

PixelConverter::PixelConverter(void)
: m_convertMode(NO_CONVERT),
  m_dstFrameBuffer(0)
{
}

PixelConverter::~PixelConverter(void)
{
  reset();
}

void PixelConverter::convert(const ::int_rectangle &  rect, FrameBuffer *dstFb,
                             const FrameBuffer *srcFb) const
{
  if (m_convertMode == NO_CONVERT) {
    dstFb->copyFrom(rect, srcFb, rect.left, rect.top);
  } else {
    int rectHeight = rect.height();
    int rectWidth = rect.width();
    int fbWidth = dstFb->getDimension().cx;
    PixelFormat dstPf = dstFb->getPixelFormat();
    PixelFormat srcPf = srcFb->getPixelFormat();

    unsigned int dstPixelSize = dstPf.bitsPerPixel / 8;
    unsigned int srcPixelSize = srcPf.bitsPerPixel / 8;

    // FIXME: Make FrameBuffer do the math.
    unsigned char *dstPixP = (unsigned char *)dstFb->getBuffer() +
                     (fbWidth * rect.top + rect.left) * dstPixelSize;
    unsigned char *srcPixP = (unsigned char *)srcFb->getBuffer() +
                     (fbWidth * rect.top + rect.left) * srcPixelSize;
    if (m_convertMode == CONVERT_FROM_16) {
      for (int i = 0; i < rectHeight; i++,
           dstPixP += (fbWidth - rectWidth) * dstPixelSize,
           srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
        for (int j = 0; j < rectWidth; j++,
                                       dstPixP += dstPixelSize,
                                       srcPixP += srcPixelSize) {
          unsigned int dstPixel = m_hexBitsTable[*(unsigned short *)srcPixP];
          if (dstPixelSize == 4) {
            *(unsigned int *)dstPixP = (unsigned int)dstPixel;
          } else if (dstPixelSize == 2) {
            *(unsigned short *)dstPixP = (unsigned short)dstPixel;
          } else if (dstPixelSize == 1) {
            *(unsigned char *)dstPixP = (unsigned char)dstPixel;
          }
        }
      }
    } else if (m_convertMode == CONVERT_FROM_32) {
      bool bigEndianDiffs = dstPf.bigEndian != srcPf.bigEndian;
      unsigned int srcRedMax = srcPf.redMax;
      unsigned int srcGrnMax = srcPf.greenMax;
      unsigned int srcBluMax = srcPf.blueMax;

      for (int i = 0; i < rectHeight; i++,
           dstPixP += (fbWidth - rectWidth) * dstPixelSize,
           srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
        for (int j = 0; j < rectWidth; j++,
                                       dstPixP += dstPixelSize,
                                       srcPixP += srcPixelSize) {
          unsigned int dstPixel = m_redTable[*(unsigned int *)srcPixP >>
                                       srcPf.redShift & srcRedMax] |
                            m_grnTable[*(unsigned int *)srcPixP >>
                                       srcPf.greenShift & srcGrnMax] |
                            m_bluTable[*(unsigned int *)srcPixP >>
                                       srcPf.blueShift & srcBluMax];
          if (dstPixelSize == 4) {
            *(unsigned int *)dstPixP = dstPixel;
            if (bigEndianDiffs) {
              *(unsigned int *)dstPixP = rotateUint32(*(unsigned int *)dstPixP);
            }
          } else if (dstPixelSize == 2) {
            *(unsigned short *)dstPixP = dstPixel;
            if (bigEndianDiffs) {
              *(unsigned short *)dstPixP = *(unsigned short *)dstPixP << 8 |
                                   *(unsigned short *)dstPixP >> 8;
            }
          } else if (dstPixelSize == 1) {
            *(unsigned char *)dstPixP = dstPixel;
          }
        }
      } 
    } else {
      _ASSERT(0);
    }
  }
}

const FrameBuffer *
PixelConverter::convert(const ::int_rectangle &  rect, const FrameBuffer *srcFb)
{
  if (m_convertMode == NO_CONVERT) {
    return srcFb;
  }

  const ::int_size fbSize = srcFb->getDimension();
  if (m_dstFrameBuffer == 0) {
    // No frame buffer allocated - construct new one from the scratch.
    m_dstFrameBuffer = new FrameBuffer;
    m_dstFrameBuffer->setProperties(fbSize, m_dstFormat);
  } else if (m_dstFrameBuffer->getDimension() != fbSize)
  {
    // Frame buffer is allocated but its size it wrong - just resize it.
    // Note that if the frame buffer is allocated, its pixel format is
    // guaranteed to be relevant, because setPixelFormats() always calls
    // reset() if at least one pixel format has been changed.
    m_dstFrameBuffer->setDimension(fbSize);
  }

  // Finally, convert pixels.
  convert(rect, m_dstFrameBuffer, srcFb);
  return m_dstFrameBuffer;
}

void PixelConverter::reset()
{
  // Deallocate the framebuffer.
  if (m_dstFrameBuffer != 0) {
    delete m_dstFrameBuffer;
    m_dstFrameBuffer = 0;
  }
}

void PixelConverter::setPixelFormats(const PixelFormat & dstPf,
                                     const PixelFormat & srcPf)
{
  if (srcPf != m_srcFormat || dstPf != m_dstFormat) {
    // Reset both translation tables and the internal frame buffer.
    reset();

    if (srcPf == dstPf) {
      m_convertMode = NO_CONVERT;
    } else if (srcPf.bitsPerPixel == 16) { // 16 bit -> N
      m_convertMode = CONVERT_FROM_16;
      fillHexBitsTable(dstPf, srcPf);
    } else if (srcPf.bitsPerPixel == 32) { // 32 bit -> N
      m_convertMode = CONVERT_FROM_32;
      fill32BitsTable(dstPf, srcPf);
    }

    m_srcFormat = srcPf;
    m_dstFormat = dstPf;
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

void PixelConverter::fillHexBitsTable(const PixelFormat & dstPf,
                                      const PixelFormat & srcPf)
{
  m_hexBitsTable.resize(65536);

  unsigned int dstRedMax = dstPf.redMax;
  unsigned int dstGrnMax = dstPf.greenMax;
  unsigned int dstBluMax = dstPf.blueMax;

  unsigned int dstRedShift = dstPf.redShift;
  unsigned int dstGrnShift = dstPf.greenShift;
  unsigned int dstBluShift = dstPf.blueShift;

  unsigned int srcRedMax = srcPf.redMax;
  unsigned int srcGrnMax = srcPf.greenMax;
  unsigned int srcBluMax = srcPf.blueMax;

  unsigned int srcRedMask = srcRedMax << srcPf.redShift;
  unsigned int srcGrnMask = srcGrnMax << srcPf.greenShift;
  unsigned int srcBluMask = srcBluMax << srcPf.blueShift;

  for (unsigned int i = 0; i < 65536; i++) {
    // Get source color component
    unsigned int srcRed = (i & srcRedMask) >> srcPf.redShift;
    unsigned int srcGrn = (i & srcGrnMask) >> srcPf.greenShift;
    unsigned int srcBlu = (i & srcBluMask) >> srcPf.blueShift;

    unsigned int dstRed = (srcRed * dstRedMax / srcRedMax) << dstRedShift;
    unsigned int dstGrn = (srcGrn * dstGrnMax / srcGrnMax) << dstGrnShift;
    unsigned int dstBlu = (srcBlu * dstBluMax / srcBluMax) << dstBluShift;
    m_hexBitsTable[i] = dstRed | dstGrn | dstBlu;
    if (dstPf.bigEndian != srcPf.bigEndian) {
      if (dstPf.bitsPerPixel == 32) {
        m_hexBitsTable[i] = rotateUint32(m_hexBitsTable[i]);
      }
      else if (dstPf.bitsPerPixel == 16) {
        m_hexBitsTable[i] = (m_hexBitsTable[i] & 0xff) << 8 | (m_hexBitsTable[i] & 0xff00) >> 8;
      }
    }
  }
}

void PixelConverter::fill32BitsTable(const PixelFormat & dstPf,
                                     const PixelFormat & srcPf)
{
  unsigned int dstRedMax = dstPf.redMax;
  unsigned int dstGrnMax = dstPf.greenMax;
  unsigned int dstBluMax = dstPf.blueMax;

  unsigned int dstRedShift = dstPf.redShift;
  unsigned int dstGrnShift = dstPf.greenShift;
  unsigned int dstBluShift = dstPf.blueShift;

  unsigned int srcRedMax = srcPf.redMax;
  unsigned int srcGrnMax = srcPf.greenMax;
  unsigned int srcBluMax = srcPf.blueMax;

  m_redTable.resize(srcRedMax + 1);
  m_grnTable.resize(srcGrnMax + 1);
  m_bluTable.resize(srcBluMax + 1);

  for (unsigned int i = 0; i <= srcRedMax; i++) {
    m_redTable[i] = ((i * dstRedMax + srcRedMax / 2) / srcRedMax) << dstRedShift;
  }
  for (unsigned int i = 0; i <= srcGrnMax; i++) {
    m_grnTable[i] = ((i * dstGrnMax + srcGrnMax / 2) / srcGrnMax) << dstGrnShift;
  }
  for (unsigned int i = 0; i <= srcBluMax; i++) {
    m_bluTable[i] = ((i * dstBluMax + srcBluMax / 2) / srcBluMax) << dstBluShift;
  }
}

unsigned int PixelConverter::rotateUint32(unsigned int value) const
{
  unsigned int result;
  char *src = (char *)&value;
  char *dst = (char *)&result;
  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];

  return result;
}
