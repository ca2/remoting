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
#include "HextileEncoder.h"
#include "HextileTile.h"

#include <crtdbg.h>


HextileEncoder::HextileEncoder(PixelConverter *conv, DataOutputStream *output)
: Encoder(conv, output)
{
}

HextileEncoder::~HextileEncoder()
{
}

int HextileEncoder::getCode() const
{
  return EncodingDefs::HEXTILE;
}

void HextileEncoder::sendRectangle(const ::int_rectangle &  rect,
                                   const FrameBuffer *serverFb,
                                   const EncodeOptions *options)
{
  const FrameBuffer *fb = m_pixelConverter->convert(rect, serverFb);

  size_t bpp = fb->getBitsPerPixel();
  if (bpp == 8) {
    hextileFunction<unsigned char>(rect, fb);
  } else if (bpp == 16) {
    hextileFunction<unsigned short>(rect, fb);
  } else if (bpp == 32) {
    hextileFunction<unsigned int>(rect, fb);
  } else {
    _ASSERT(0);
  }
}

template <class PIXEL_T>
void HextileEncoder::hextileFunction(const ::int_rectangle &r,
                                     const FrameBuffer *frameBuffer)
{
  ::int_rectangle t;
  PIXEL_T *buf;
  FrameBuffer fb;
  PIXEL_T oldBg = 0, oldFg = 0;
  bool oldBgValid = false;
  bool oldFgValid = false;
  unsigned char encoded[256 * sizeof(PIXEL_T)];

  HextileTile<PIXEL_T> tile;

  for (t.top = r.top; t.top < r.bottom; t.top += 16) {

    t.bottom = min(r.bottom, t.top + 16);

    for (t.left = r.left; t.left < r.right; t.left += 16) {

      t.right = min(r.right, t.left + 16);

      fb.setProperties(t, frameBuffer->getPixelFormat());
      fb.copyFrom(frameBuffer, t.left, t.top);
      buf = (PIXEL_T *)fb.getBuffer();

      tile.newTile(buf, t.width(), t.height());
      int tileType = tile.getFlags();
      size_t encodedLen = tile.getSize();

      if ( (tileType & e_hextile_raw) != 0 ||
           encodedLen >= t.width() * t.height() * sizeof(PIXEL_T) ) {
        m_output->writeUInt8(e_hextile_raw);
        m_output->writeFully((char *)buf, t.width() * t.height() * sizeof(PIXEL_T));
        oldBgValid = oldFgValid = false;
        continue;
      }

      PIXEL_T bg = tile.getBackground();
      PIXEL_T fg = 0;

      if (!oldBgValid || oldBg != bg) {
        tileType |= e_hextile_bg_specified;
        oldBg = bg;
        oldBgValid = true;
      }

      if (tileType & e_hextile_any_subrects) {
        if (tileType & e_hextile_subrects_coloured) {
          oldFgValid = false;
        } else {
          fg = tile.getForeground();
          if (!oldFgValid || oldFg != fg) {
            tileType |= e_hextile_fg_specified;
            oldFg = fg;
            oldFgValid = true;
          }
        }
        tile.encode(encoded);
      }

      m_output->writeUInt8(tileType);
      if (tileType & e_hextile_bg_specified) {
        m_output->writeFully(&bg, sizeof(PIXEL_T));
      }
      if (tileType & e_hextile_fg_specified) {
        m_output->writeFully(&fg, sizeof(PIXEL_T));
      }
      if (tileType & e_hextile_any_subrects) {
        m_output->writeFully(encoded, encodedLen);
      }
    }
  }
}
