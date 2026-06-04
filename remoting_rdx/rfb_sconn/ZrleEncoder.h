// Copyright (C) 2013 GlavSoft LLC.
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

#include "Encoder.h"
#include "TightPalette.h"
#include "subsystem/platform/Deflater.h"

namespace remoting_rfb
{


   class ZrleEncoder : public Encoder
   {
   public:
      ZrleEncoder(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream);
      virtual ~ZrleEncoder();

      // Follow methods were inherited from the Encoder.
      virtual ::i32 getCode() const;

      virtual void splitRectangle(const ::i32_rectangle & rectangle, ::i32_rectangle_array_base & rectanglea,
                                  const ::innate_subsystem::Framebuffer *serverFb, const EncodeOptions *options);

      virtual void sendRectangle(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *serverFb,
                                 const EncodeOptions *options);

   private:
      // Determine the class of rectangle and call necessary function for this type.
      template<class PIXEL_T>
      void sendRect(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *serverFb,
                    const ::innate_subsystem::Framebuffer *clientFb, const EncodeOptions *options);

      // Send raw tile.
      template<class PIXEL_T>
      void writeRawTile(const ::i32_rectangle &tileRect, const ::innate_subsystem::Framebuffer *pframebuffer);

      // Send a solid-color tile.
      void writeSolidTile();

      // Send packed palette tile.
      template<class PIXEL_T>
      void writePackedPaletteTile(const ::i32_rectangle &tileRect, const ::innate_subsystem::Framebuffer *pframebuffer);

      // Send palette RLE tile.
      template<class PIXEL_T>
      void writePaletteRleTile(const ::i32_rectangle &tileRect, const ::innate_subsystem::Framebuffer *pframebuffer);

      // Write data from runLength (used in plain Rle encoding).
      void pushRunLengthRle(::i32 runLength);

      // Write data from runLength (used in palette Rle encoding).
      void pushRunLengthPaletteRle(::i32 runLength, ::array_base<::u8> *paletteRleData);

      // Write pixel to the plainRleTile.
      template<class PIXEL_T>
      void writePixelToPlainRleTile(const PIXEL_T px, PIXEL_T *previousPx);

      // Fill palette (m_pal), create m_plainRleTile ::array_base and calculate size of data in palette RLE tile.
      template<class PIXEL_T>
      void fillPalette(const ::i32_rectangle &tileRect, const ::innate_subsystem::Framebuffer *pframebuffer);

      // Copy ordinary PIXELs.
      template<class PIXEL_T>
      void copyPixels(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *pframebuffer, ::u8 *dst);

      // Copy CPIXELs.
      void copyCPixels(const ::i32_rectangle & rectangle, const ::innate_subsystem::Framebuffer *pframebuffer, ::u8 *dst);

      // Vector for storing all tiles for the future zlib compression.
      ::array_base<::u8> m_rgbData;
      // Size of m_rgbData before writing information in it.
      size_t m_oldSize;

      // Size of stride.
      ::i32 m_fbWidth;

      // Size of packed pixels in palette.
      ::i32 m_mSize;

      // The only pixel format type for whole rectangle.
      ::innate_subsystem::PixelFormat m_pxFormat;

      // Used for determing: is it CPIXEL or PIXEL.
      size_t m_bytesPerPixel;
      size_t m_numberFirstByte;

      // Zlib object and settings for the it.
      ::subsystem::Deflater m_deflater;
      ::i32 m_idxZlibLevel;
      ::i32 m_monoZlibLevel;
      ::i32 m_rawZlibLevel;

      // Color palette.
      TightPalette m_pal;

      // Variables for storing size of all tiles.
      size_t m_rawTileSize;
      size_t m_paletteTileSize;
      size_t m_paletteRleTileSize;

      // ::array_base for storing plain RLE tile data
      ::array_base<::u8> m_plainRleTile;

   private:
      // Tile size in ZRLE encoding by default.
      static const ::i32 TILE_SIZE = 64;

      // Default values for zlib settings.
      static const ::i32 ZLIB_IDX_LEVEL_DEFAULT = 7;
      static const ::i32 ZLIB_MONO_LEVEL_DEFAULT = 7;
      static const ::i32 ZLIB_RAW_LEVEL_DEFAULT = 6;

      // Description of tile max size is located in ZrleDecoder.h.
      // The max possible size of tile is 20481, so 20482 is not possible.
      static const ::i32 THIS_TYPE_OF_TILE_IS_NOT_POSSIBLE = 20482;

      // Max possible colors in palette (127 is max for RLE palette type encoding).
      static const ::u8 MAX_NUMBER_OF_COLORS_IN_PALETTE = 127;
   };

   

} // namespace remoting_rfb



