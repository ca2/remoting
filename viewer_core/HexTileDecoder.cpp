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
#include "HexTileDecoder.h"

#include <algorithm>

HexTileDecoder::HexTileDecoder(LogWriter *logWriter)
: DecoderOfRectangle(logWriter)
{
  m_encoding = EncodingDefs::HEXTILE;
}

HexTileDecoder::~HexTileDecoder()
{
}

void HexTileDecoder::decode(RfbInputGate *pinput,
                            FrameBuffer *pframebuffer,
                            const ::int_rectangle &  dstRect)
{
  // shorcut
  const int bytesPerPixel = pframebuffer->getBytesPerPixel();

  unsigned int background = 0;
  unsigned int foreground = 0;

  bool backgroundAccepted = false;

  for (int y = dstRect.top; y < dstRect.bottom; y += TILE_SIZE) {
    for (int x = dstRect.left; x < dstRect.right; x += TILE_SIZE) {
      ::int_rectangle tileRect(x,
                    y,
                    ::minimum(x + TILE_SIZE, dstRect.right),
                    ::minimum(y + TILE_SIZE, dstRect.bottom));

      if (::int_rectangle(pframebuffer->getDimension()).intersection(tileRect) != tileRect)
        throw ::remoting::Exception("Error in protocol: incorrect size of tile in hextile-decoder");

      unsigned char flags = pinput->readUInt8();
      // If tile-coding is RAW.
      if (flags & 0x1) {
        for (int y = tileRect.top; y < tileRect.bottom; y++)
          pinput->readFully(pframebuffer->getBufferPtr(tileRect.left, y),
                           tileRect.width() * bytesPerPixel);
      } else {
        if (flags & 0x2) {
          pinput->readFully(&background, bytesPerPixel);
          backgroundAccepted = true;
        }

        if (backgroundAccepted)
          pframebuffer->fillRect(tileRect, background);

        if (flags & 0x4)
          pinput->readFully(&foreground, bytesPerPixel);

        if (flags & 0x8) {
          unsigned char numberOfSubrectangles = pinput->readUInt8();

          for (int i = 0; i < numberOfSubrectangles; i++) {

            if (flags & 0x10 && !(flags & 0x4))
              pinput->readFully(&foreground, bytesPerPixel);

            unsigned char xy = pinput->readUInt8();
            unsigned char wh = pinput->readUInt8();
            int x = (xy >> 4) & 0xF;
            int y = xy & 0xF;
            int w = ((wh >> 4) & 0xF) + 1;
            int h = (wh & 0xF) + 1;
            ::int_rectangle subRect(x, y, x + w, y + h);

            subRect.offset(tileRect.left, tileRect.top);
            pframebuffer->fillRect(subRect, foreground);
          }
        } else { // exist subrect
          if (!backgroundAccepted)
            throw ::remoting::Exception("Server error in HexTile encoding: background color not accepted");
        }
      } // it tile is not RAW
    } // for each tiles in line
  } // for each line of tile
}
