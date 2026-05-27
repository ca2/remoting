// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "HexTileDecoder.h"
#include "subsystem/platform/Exception.h"

//#include aaa_<algorithm>

namespace remoting_client
{
   HexTileDecoder::HexTileDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter)
   {
      m_encoding = ::remoting_rtc::EncodingDefs::HEXTILE;
   }

   HexTileDecoder::~HexTileDecoder()
   {
   }

   void HexTileDecoder::decode(::remoting_rtc::RfbInputGate *pinput,
                               ::innate_subsystem::Framebuffer *pframebuffer,
                               const ::i32_rectangle &  rectangleTarget)
   {
      // shorcut
      const ::i32 bytesPerPixel = pframebuffer->getBytesPerPixel();

      ::u32 background = 0;
      ::u32 foreground = 0;

      bool backgroundAccepted = false;

      for (::i32 y = rectangleTarget.top; y < rectangleTarget.bottom; y += TILE_SIZE) {
         for (::i32 x = rectangleTarget.left; x < rectangleTarget.right; x += TILE_SIZE) {
            ::i32_rectangle tileRect(x,
                          y,
                          ::minimum(x + TILE_SIZE, rectangleTarget.right),
                          ::minimum(y + TILE_SIZE, rectangleTarget.bottom));

            if (::i32_rectangle(pframebuffer->getDimension()).intersection(tileRect) != tileRect)
               throw ::subsystem::Exception("Error in protocol: incorrect size of tile in hextile-decoder");

            ::u8 flags = pinput->readUInt8();
            // If tile-coding is RAW.
            if (flags & 0x1) {
               for (::i32 y = tileRect.top; y < tileRect.bottom; y++)
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
                  ::u8 numberOfSubrectangles = pinput->readUInt8();

                  for (::i32 i = 0; i < numberOfSubrectangles; i++) {

                     if (flags & 0x10 && !(flags & 0x4))
                        pinput->readFully(&foreground, bytesPerPixel);

                     ::u8 xy = pinput->readUInt8();
                     ::u8 wh = pinput->readUInt8();
                     ::i32 x = (xy >> 4) & 0xF;
                     ::i32 y = xy & 0xF;
                     ::i32 w = ((wh >> 4) & 0xF) + 1;
                     ::i32 h = (wh & 0xF) + 1;
                     ::i32_rectangle subRect(x, y, x + w, y + h);

                     subRect.offset(tileRect.left, tileRect.top);
                     pframebuffer->fillRect(subRect, foreground);
                  }
               } else { // exist subrect
                  if (!backgroundAccepted)
                     throw ::subsystem::Exception("Server error in HexTile encoding: background color not accepted");
               }
            } // it tile is not RAW
         } // for each tiles in line
      } // for each line of tile
   }
} // namespace remoting_client