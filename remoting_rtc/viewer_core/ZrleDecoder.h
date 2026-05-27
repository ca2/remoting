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

#pragma once


#include "DecoderOfRectangle.h"

#include "acme/input_output/DataInputStream.h"
#include "subsystem/platform/Inflater.h"

namespace remoting_client
{
   class CLASS_DECL_REMOTING ZrleDecoder : public DecoderOfRectangle
   {
   public:



      typedef ::array_base<::u32> Palette;
      ::array_base<::i8> m_pixels;
      ::array_base<::i8> m_zlibDataReadAndInflate;
      ::memory m_unpackedData;
      ::subsystem::Inflater m_inflater;
      size_t m_bytesPerPixel;
      size_t m_numberFirstByte;


      ZrleDecoder(::subsystem::LogWriter * plogwriter);
      virtual ~ZrleDecoder();

   //protected:

   //protected:
      virtual void decode(::remoting_rtc::RfbInputGate *input,
                          ::innate_subsystem::Framebuffer *pframebuffer,
                          const ::i32_rectangle &  rectangleTarget);


      void readAndInflate(::remoting_rtc::RfbInputGate *input, size_t maximalUnpackedSize);

      ::i32 readType(::DataInputStream * pinput);

      size_t readRunLength(::DataInputStream * pinput);

      void readPalette(::DataInputStream * pinput,
                       const ::i32 paletteSize,
                       Palette *palette);

      void readRawTile(::DataInputStream * pinput,
                       ::array_base<::i8> &pixels,
                       const ::i32_rectangle &  tileRect);

      void readSolidTile(::DataInputStream * pinput,
                         ::array_base<::i8> &pixels,
                         const ::i32_rectangle &  tileRect);

      void readPackedPaletteTile(::DataInputStream * pinput,
                                 ::array_base<::i8> &pixels,
                                 const ::i32_rectangle &  tileRect,
                                 const ::i32 type);

      void readPlainRleTile(::DataInputStream * pinput,
                            ::array_base<::i8> &pixels,
                            const ::i32_rectangle &  tileRect);

      void readPaletteRleTile(::DataInputStream * pinput,
                              ::array_base<::i8> &pixels,
                              const ::i32_rectangle &  tileRect,
                              const ::i32 type);


      void drawTile(::innate_subsystem::Framebuffer *pframebuffer,
                    const ::i32_rectangle &  tileRect,
                    const ::array_base<::i8> *pixels);


   private:
      static const ::i32 TILE_SIZE = 64;

      // maximal size of tile (64x64) is max value from follow:
      // 1. size of raw: subenc + data:
      //    1 + width * height * pixelSize = 1 + 64 * 64 * 4 = 16385
      // 2. size of solid: subenc + solidColor:
      //    1 + pixelSize = 1 + 4 = 5
      // 3. size of packpalette: subenc + palleteSize + data:
      //    1 + paletteSize * pixelSize + m = 1 + 16 * 4 + 32 * 64 = 2113
      // 4. size of plainRle: subenc + rle * width * height:
      //    1 + (pixelSize + 1) * width * height = 1 + (4 + 1) * 64 * 64 = 20481
      // 5. size of paletteRle: subenc + paletteSzie + data:
      //    1 + paletteSize * pixelSize + rle * width * height = 1 + 128 * 4 + (1 + 1) * 64 * 64 = 8705
      static const size_t MAXIMAL_TILE_SIZE = 20481;
      static const size_t TILE_LENGTH_SIZE = sizeof(::u32);

      static size_t getMaxSizeOfRectangle(const ::i32_rectangle &  rectangleTarget);
   };
} //namespace remoting_client