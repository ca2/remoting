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


//#include aaa_<vector>

#include "subsystem/platform/Inflater.h"

#include "DecoderOfRectangle.h"
#include "JpegDecompressor.h"

namespace remoting_rfb_client
{
   class CLASS_DECL_REMOTING_RDX TightDecoder : public DecoderOfRectangle
   {
   public:
      TightDecoder(::subsystem::LogWriter * plogwriter);
      virtual ~TightDecoder();

   protected:
      void decode(::remoting_rfb::RfbInputGate *input,
                          ::innate_subsystem::Framebuffer *pframebuffer,
                          const ::i32_rectangle &  rectangleTarget) override;

   private:
      void reset();
      void resetDecoders(unsigned char compControl);
      ::u32 readTightPixel(::remoting_rfb::RfbInputGate *input, int bytesPerCPixel);
      int readCompactSize(::remoting_rfb::RfbInputGate *input);
      ::array_base<::u32> readPalette(::remoting_rfb::RfbInputGate *input,
                              int paletteSize,
                              int bytesPerCPixel);
      void processJpeg(::remoting_rfb::RfbInputGate *input,
                       ::innate_subsystem::Framebuffer *pframebuffer,
                       const ::i32_rectangle &  rectangleTarget);
      void processBasicTypes(::remoting_rfb::RfbInputGate *input,
                             ::innate_subsystem::Framebuffer *pframebuffer,
                             const ::i32_rectangle &  rectangleTarget,
                             unsigned char compControl);
      void readTightData(::remoting_rfb::RfbInputGate *input,
                         ::array_base<unsigned char> &buffer,
                         size_t expectedLength,
                         const int decoderId);
      void readCompressedData(::remoting_rfb::RfbInputGate *input,
                              ::array_base<unsigned char> &buffer,
                              size_t expectedLength,
                              const int decoderId);
      void drawPalette(::innate_subsystem::Framebuffer *pframebuffer,
                       const ::array_base<::u32> &palette,
                       const ::array_base<unsigned char> &pixels,
                       const ::i32_rectangle &  rectangleTarget);
      void drawGradient(::innate_subsystem::Framebuffer *pframebuffer,
                        const ::array_base<unsigned char> &pixels,
                        const ::i32_rectangle &  rectangleTarget);
      void drawTightBytes(::innate_subsystem::Framebuffer *pframebuffer,
                         const ::array_base<unsigned char> *pixels,
                         const ::i32_rectangle &  rectangleTarget);
      void drawJpegBytes(::innate_subsystem::Framebuffer *pframebuffer,
                         const ::array_base<unsigned char> *pixels,
                         const ::i32_rectangle &  rectangleTarget);

      ::u32 getRawTightColor(const ::innate_subsystem::PixelFormat & pixelformat,
                              const ::array_base<unsigned char> &pixels,
                              size_t offset);
      void fillRawComponents(const ::innate_subsystem::PixelFormat & pixelformat,
                             unsigned char components[],
                             const ::array_base<unsigned char> &pixels,
                             size_t pixelOffset);

      ::u32 transformPixelToTight(::u32 color);
      ::array_base<unsigned char> transformArray(const ::array_base<unsigned char> &buffer);

      ::pointer_array_base<::subsystem::Inflater > m_inflater;
      JpegDecompressor m_jpeg;

      bool m_isCPixel;
   private:
      static const int MAX_SUBENCODING = 0x09;
      static const int JPEG_TYPE = 0x09;
      static const int FILL_TYPE = 0x08;

      static const int FILTER_ID_MASK = 0x40;
      // TODO: removed 0x30 constant?
      static const int STREAM_ID_MASK = 0x30;

      static const int COPY_FILTER = 0x00;
      static const int PALETTE_FILTER = 0x01;
      static const int GRADIENT_FILTER = 0x02;

      static const int DECODERS_NUM = 4;

      static const int MIN_SIZE_TO_COMPRESS = 12;
   };
} //namespace remoting_rfb_client
