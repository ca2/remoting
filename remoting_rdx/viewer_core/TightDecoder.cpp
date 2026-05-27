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
#include "TightDecoder.h"

#include "subsystem/framebuffer/StandardPixelFormatFactory.h"

namespace remoting_rfb_client
{
   TightDecoder::TightDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter),
     m_isCPixel(false)
   {
      m_encoding = ::remoting_rfb::EncodingDefs::TIGHT;

      m_inflater.resize(DECODERS_NUM);
      for (::i32 i = 0; i < DECODERS_NUM; i++)
         m_inflater[i] = allocateø ::subsystem::Inflater;
      reset();
   }

   TightDecoder::~TightDecoder()
   {
//      for (::i32 i = 0; i < DECODERS_NUM; i++) {
//         try {
//            delete m_inflater[i];
//         } catch (...) {
//         }
//      }
   }

   void TightDecoder::decode(::remoting_rfb::RfbInputGate *pinput,
                             ::innate_subsystem::Framebuffer *pframebuffer,
                             const ::i32_rectangle &  rectangleTarget)
   {
      // The width of any Tight-encoded rectangle cannot exceed 2048
      // pixels. If a rectangle is wider, it must be split into several rectangles
      // and each one should be encoded separately.

      m_isCPixel = false;
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      if (pixelformat.colorDepth == 24 && pixelformat.bitsPerPixel == 32 &&
          pixelformat.redMax == 255 && pixelformat.greenMax == 255 && pixelformat.blueMax == 255) {
         m_isCPixel = true;
          }

      ::u8 compressionControl = pinput->readUInt8();
      resetDecoders(compressionControl);
      ::u8 compressionType = (compressionControl >> 4) & 0x0F;

      ::i32 bytesPerCPixel = pframebuffer->getBytesPerPixel();

      if (compressionType > MAX_SUBENCODING) {
         throw ::subsystem::Exception("Sub-encoding in Tight-encoder are not valid");
      }
      if (::i32_rectangle(pframebuffer->getDimension()).intersection(rectangleTarget)!= rectangleTarget)
         throw ::subsystem::Exception("Error in protocol: incorrect size of rectangle (tight-decoder)");

      if (compressionType == FILL_TYPE) {
         ::u32 color = readTightPixel(pinput, bytesPerCPixel);
         pframebuffer->fillRect(rectangleTarget, color);
      } else if (compressionType == JPEG_TYPE) {
         processJpeg(pinput, pframebuffer, rectangleTarget);
      } else
         processBasicTypes(pinput, pframebuffer, rectangleTarget, compressionControl);
   }

   ::u32 TightDecoder::transformPixelToTight(::u32 color)
   {
      ::u32 result = 0;
      result |= (color & 0xFF) << 16;
      result |= color & 0xFF00;
      result |= (color >> 16);
      return result;
   }

   ::array_base<::u8> TightDecoder::transformArray(const ::array_base<::u8> &buffer)
   {
      ::array_base<::u8> result(buffer.size() * 4 / 3);
      for (size_t bi = 0, ri = 0; bi < buffer.size(); bi += 3, ri += 4) {
         result[ri] = buffer[bi + 2];
         result[ri + 1] = buffer[bi + 1];
         result[ri + 2] = buffer[bi];
         //result[ri + 3] = 0;
         result[ri + 3] = 255;
      }
      return result;
   }

   ::u32 TightDecoder::readTightPixel(::remoting_rfb::RfbInputGate *pinput, ::i32 bytesPerCPixel)
   {
      ::u32 color = 0;
      ::u8 buffer[sizeof(color)];
      if (!m_isCPixel) {
         pinput->readFully(buffer, bytesPerCPixel);
      } else {
         pinput->readFully(buffer, 3);
         ::u8 t = buffer[2];
         buffer[2] = buffer[0];
         buffer[0] = t;
         //buffer[3] = 0;
         buffer[3] = 255;
      }
      memcpy(&color, buffer, bytesPerCPixel);
      return color;
   }

   void TightDecoder::reset()
   {
      for (::i32 i = 0; i < DECODERS_NUM; i++) {
         //delete m_inflater[i];
         m_inflater[i] = allocateø ::subsystem::Inflater;
      }
   }

   void TightDecoder::resetDecoders(::u8 compressionControl)
   {
      for (::i32 i = 0; i < DECODERS_NUM; i++)
         if (compressionControl & (0x01 << i)) {
            //delete m_inflater[i];
            m_inflater[i] = allocateø ::subsystem::Inflater;
         }
   }

   ::i32 TightDecoder::readCompactSize(::remoting_rfb::RfbInputGate *pinput)
   {
      ::i32 b = pinput->readUInt8();
      ::i32 size = b & 0x7F;
      if ((b & 0x80) != 0) {
         b = pinput->readUInt8();
         size += (b & 0x7F) << 7;
         if ((b & 0x80) != 0) {
            size += pinput->readUInt8() << 14;
         }
      }
      return size;
   }

   void TightDecoder::processJpeg(::remoting_rfb::RfbInputGate *pinput,
                                  ::innate_subsystem::Framebuffer *pframebuffer,
                                  const ::i32_rectangle &  rectangleTarget)
   {
      ::u32 jpegBufLen = readCompactSize(pinput);
      if (jpegBufLen == 0)
         throw ::subsystem::Exception("Error in protocol: empty byffer of jpeg (tight-decoder)");
      ::array_base<::u8> buffer;
      buffer.resize(jpegBufLen);
      pinput->readFully(buffer.data(), jpegBufLen);

      if (rectangleTarget.area() != 0) {
         ::array_base<::u8> pixels;
         pixels.resize(rectangleTarget.area() * JpegDecompressor::BYTES_PER_PIXEL);

         try {
            m_jpeg.decompress(buffer, jpegBufLen, pixels, rectangleTarget);
            if (m_isCPixel) {
               pixels = transformArray(pixels);
               drawTightBytes(pframebuffer, &pixels, rectangleTarget);
            } else {
               drawJpegBytes(pframebuffer, &pixels, rectangleTarget);
            }
         } catch (const ::subsystem::Exception &ex) {
            ::string error;
            error.format("Error in tight-decoder, subencoding \"jpeg\": {}",
                         ex.get_message());
            m_plogwriter->error(error);
         }
      }
   }

   void TightDecoder::processBasicTypes(::remoting_rfb::RfbInputGate *pinput,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                                        const ::i32_rectangle &  rectangleTarget,
                                        ::u8 compressionControl)
   {
      ::i32 decoderId = (compressionControl & STREAM_ID_MASK) >> 4;
      ::i32 filterId = COPY_FILTER;
      if ((compressionControl & FILTER_ID_MASK) != 0) {
         filterId = pinput->readUInt8();
      }

      ::i32 bytesPerCPixel = pframebuffer->getBytesPerPixel();
      size_t lengthCurrentBpp = rectangleTarget.area() * bytesPerCPixel;
      if (m_isCPixel) {
         lengthCurrentBpp = rectangleTarget.area() * 3;
      }

      ::array_base<::u8> buffer;

      switch (filterId) {
         case COPY_FILTER:
            information("COPY_FILTER");
            readTightData(pinput, buffer, lengthCurrentBpp, decoderId);
            informationf(
                "decoded bytes=%zu expected=%zu",
                buffer.size(),
                lengthCurrentBpp);
            if (m_isCPixel) {
               buffer = transformArray(buffer);
            }
            drawTightBytes(pframebuffer, &buffer, rectangleTarget);
            break;

            // The "gradient" filter and "jpeg" compression may be used only
            // when bits-per-pixel value is either 16 or 32, not 8.
         case PALETTE_FILTER:
         {
            information("PALETTE_FILTER");
            ::i32 paletteSize = pinput->readUInt8() + 1;
            ::array_base<::u32> palette = readPalette(pinput, paletteSize, bytesPerCPixel);
            size_t dataLength = rectangleTarget.area();
            if (paletteSize == 2) {
               dataLength = (rectangleTarget.width() + 7) / 8 * rectangleTarget.height();
            }
            readTightData(pinput, buffer, dataLength, decoderId);
            drawPalette(pframebuffer, palette, buffer, rectangleTarget);
         }
            break;

         case GRADIENT_FILTER:
            information("GRADIENT_FILTER");
            readTightData(pinput, buffer, lengthCurrentBpp, decoderId);
            drawGradient(pframebuffer, buffer, rectangleTarget);
            break;

         default:
            break;
      }
   }

   ::array_base<::u32> TightDecoder::readPalette(::remoting_rfb::RfbInputGate *pinput,
                                         ::i32 paletteSize,
                                         ::i32 bytesPerCPixel)
   {
      ::array_base<::u32> palette(paletteSize);
      for (::i32 i = 0; i < paletteSize; i++) {
         palette[i] = readTightPixel(pinput, bytesPerCPixel);
      }
      return palette;
   }

   void TightDecoder::readTightData(::remoting_rfb::RfbInputGate *pinput,
                                    ::array_base<::u8> &buffer,
                                    size_t expectedLength,
                                    const ::i32 decoderId)
   {
      if (expectedLength < MIN_SIZE_TO_COMPRESS) {
         buffer.resize(expectedLength);
         if (expectedLength != 0) {
            pinput->readFully(buffer.data(), expectedLength);
         }
      } else {
         readCompressedData(pinput, buffer, expectedLength, decoderId);
      }
   }

   void TightDecoder::readCompressedData(::remoting_rfb::RfbInputGate *pinput,
                                         ::array_base<::u8> &buffer,
                                         size_t expectedLength,
                                         const ::i32 decoderId)
   {
      size_t rawDataLength = readCompactSize(pinput);

      ::array_base<::i8> compressed(rawDataLength);

      // read compressed (raw) data behind space allocated for decompressed data
      if (rawDataLength != 0) {
         pinput->readFully(compressed.data(), rawDataLength);

         ::subsystem::Inflater *decoder = m_inflater[decoderId];
         decoder->setInput(compressed.data(), rawDataLength);
         decoder->setUnpackedSize(expectedLength);
         decoder->inflate();
         
         

         size_t size = decoder->getOutputSize();
         const_char_pointer output = decoder->getOutput();
         //buffer.resize(size);
         buffer.assign((::u8 *) output, size);
      } else {
         ASSERT(rawDataLength != 0);
         m_plogwriter->debug("Tight decoder: Length of Raw compressed data is 0");
         buffer.resize(0);
      }
   }

   void TightDecoder::drawPalette(::innate_subsystem::Framebuffer *pframebuffer,
                                  const ::array_base<::u32> &palette,
                                  const ::array_base<::u8> &pixels,
                                  const ::i32_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (draw Tight bytes)
      ::i32 width = rectangleTarget.width();
      ::i32 height = rectangleTarget.height();

      ::i32 bytesPerPixel = pframebuffer->getBytesPerPixel();

      ::i32 dstLength = rectangleTarget.area();

      ::i32 x = rectangleTarget.left;
      ::i32 y = rectangleTarget.top;
      ::i32 stride = pframebuffer->getBytesPerRow();
      char_pointer basePtr = (char_pointer )pframebuffer->getBufferPtr(x, y);
      if (palette.size() == 2) {
         ::i32 offset = 8;
         ::i32 index = -1;
         for (::i32 i = 0; i < dstLength; i++) {
            void *pixelPtr = basePtr + bytesPerPixel * (i % width) + stride * (i / width);
            if (offset == 0 || i % width == 0) {
               offset = 8;
               index++;
            }
            offset--;
            memcpy(pixelPtr, &palette[(pixels[index] >> offset) & 0x01], bytesPerPixel);
         }
      } else { // size of palette != 2
         for (::i32 i = 0; i < dstLength; i++) {
            void *pixelPtr = basePtr + bytesPerPixel * (i % width) + stride * (i / width);
            if (pixels[i] < palette.size()) {
               memcpy(pixelPtr, &palette[pixels[i]], bytesPerPixel);
            } else {
               m_plogwriter->error("Tight decoder: Invalid index in palette.");
            }
         }
      }
   }

   void TightDecoder::drawTightBytes(::innate_subsystem::Framebuffer *pframebuffer,
                                     const ::array_base<::u8> *pixels,
                                     const ::i32_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (zrle)
      ::i32 width = rectangleTarget.width();
      ::i32 height = rectangleTarget.height();

      ::i32 bytesPerPixel = pframebuffer->getBytesPerPixel();

      ::i32 dstLength = rectangleTarget.area();

      ::i32 x = rectangleTarget.left;
      ::i32 y = rectangleTarget.top;
      ::i32 iBlackArea = 0;
      ::i32 iBlackArea1 = 0;
      for (::i32 i = 0; i < dstLength; i++) {
         auto pixelPtr = (::u8*) pframebuffer->getBufferPtr(x + i % width, y + i / width);
         auto pixelSrc = (::u8*) (pixels->data() + (i * bytesPerPixel));
         auto r = pixelPtr[0];
         auto g = pixelPtr[1];
         auto b = pixelPtr[2];
         auto a = pixelPtr[3];
         auto r1 = pixelSrc[0];
         auto g1 = pixelSrc[1];
         auto b1 = pixelSrc[2];
         auto a1 = pixelSrc[3];
         if(r1 == 0 && g1 == 0 && b1 == 0)
         {
            
            iBlackArea1++;
            
         }
         else{
            
            if(iBlackArea1 > 0)
            {
               
               information("src black area = {} pixels", iBlackArea1);
               iBlackArea1 = 0;
            }
            
            
         }

         memcpy(pixelPtr, pixelSrc, bytesPerPixel);
         auto r2 = pixelPtr[0];
         auto g2 = pixelPtr[1];
         auto b2 = pixelPtr[2];
         auto a2 = pixelPtr[3];
         
         if(r2 == 0 && g2 == 0 && b2 == 0)
         {
            
            iBlackArea++;
            
         }
         else{
            
            if(iBlackArea > 0)
            {
               
               //information("black area = {} pixels", iBlackArea);
               iBlackArea = 0;
            }
            
            
         }
         if(a2 != 255)
         {
            
            information("a2 != 255");
            
         }
      }
      if(iBlackArea1 > 0)
      {
         
         information("src black area = {} pixels", iBlackArea1);
         iBlackArea1 = 0;
      }
      if(iBlackArea > 0)
      {
         
         information("dst black area = {} pixels", iBlackArea);
         iBlackArea = 0;
      }
   }

   void TightDecoder::drawJpegBytes(::innate_subsystem::Framebuffer *pframebuffer,
                                    const ::array_base<::u8> *pixels,
                                    const ::i32_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (draw tight bytes)
      ::i32 width = rectangleTarget.width();
      ::i32 height = rectangleTarget.height();

      ::i32 fbBytesPerPixel = pframebuffer->getBytesPerPixel();
      ::i32 bytesPerCPixel = 3;
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();

      ::i32 dstLength = rectangleTarget.area();

      ::i32 x = rectangleTarget.left;
      ::i32 y = rectangleTarget.top;
      for (::i32 i = 0; i < dstLength; i++) {
         ::u8 color[4] = {0, 0, 0, 0};
         memcpy(color, &pixels->operator [](i * bytesPerCPixel), bytesPerCPixel);
         ::u32 pixel = (((::u32)color[0] * pixelformat.redMax + 127) / 255 << pixelformat.redShift |
                        ((::u32)color[1] * pixelformat.greenMax + 127) / 255 << pixelformat.greenShift |
                        ((::u32)color[2] * pixelformat.blueMax + 127) / 255 << pixelformat.blueShift);

         void *pixelPtr = pframebuffer->getBufferPtr(x + i % width, y + i / width);
         memcpy(pixelPtr, &pixel, fbBytesPerPixel);
      }
   }

   /*
    *-- The "gradient" filter pre-processes pixel data with a simple algorithm
    * which converts each color component to a difference between a "predicted"
    * intensity and the actual intensity. Such a technique does not affect
    * uncompressed data size, but helps to compress photo-like images better.
    * Pseudo-code for converting intensities to differences is the following:
    *
    *   P[i,j] := V[i-1,j] + V[i,j-1] - V[i-1,j-1];
    *   if (P[i,j] < 0) then P[i,j] := 0;
    *   if (P[i,j] > MAX) then P[i,j] := MAX;
    *   D[i,j] := V[i,j] - P[i,j];
    *
    * Here V[i,j] is the intensity of a color component for a pixel at
    * coordinates (i,j). MAX is the maximum value of intensity for a color
    * component.
    */

   void TightDecoder::drawGradient(::innate_subsystem::Framebuffer *pframebuffer,
                                   const ::array_base<::u8> &pixels,
                                   const ::i32_rectangle &  rectangleTarget)
   {
      typedef ::array_base<::u16> RowType;
      size_t opRowLength = rectangleTarget.width() * 3 + 3;

      ::array_base<RowType> opRows(2);
      opRows[0].resize(opRowLength);
      opRows[1].resize(opRowLength);

      memset(opRows[0].data(), 0, opRowLength * sizeof(::u16));
      memset(opRows[1].data(), 0, opRowLength * sizeof(::u16));

      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      ::i32 fbBytesPerPixel = pframebuffer->getBytesPerPixel();
      ::i32 bytesPerCPixel = fbBytesPerPixel;
      if (m_isCPixel) {
         bytesPerCPixel = 3;
      }

      ::i32 opRowIndex = 0;
      ::u16 max[3] = {pixelformat.redMax, pixelformat.greenMax, pixelformat.blueMax};
      ::u16 shift[3] = {pixelformat.redShift, pixelformat.greenShift, pixelformat.blueShift};
      size_t pixelOffset = 0;

      for (::i32 i = 0; i < rectangleTarget.height(); ++i) {
         // exchange thisRow and prevRow:
         RowType &thisRow = opRows[opRowIndex];
         RowType &prevRow = opRows[opRowIndex = (opRowIndex + 1) % 2];

         for (size_t j = 3; j < opRowLength; j += 3, pixelOffset += bytesPerCPixel) {
            ::u8 rawColor[3];
            fillRawComponents(pixelformat, rawColor, pixels, pixelOffset);
            ::u32 color = 0;
            for (::i32 index = 0; index < 3; index++) {
               ::i32 d = prevRow[j + index] +      // "upper" pixel (from prev row)
                         thisRow[j + index - 3] -  // prev pixel
                         prevRow[j + index - 3];   // "diagonal" prev pixel
               ::u16 converted = d < 0 ? 0 : d > max[index] ? max[index] : d;
               thisRow[j + index] = (converted + rawColor[index]) & max[index];
               color |= (thisRow[j + index] & max[index]) << shift[index];
            }
            void *pixelPtr = pframebuffer->getBufferPtr(static_cast<::i32>(rectangleTarget.left + j/3 - 1),
                                              rectangleTarget.top + i);
            memcpy(pixelPtr, &color, fbBytesPerPixel);
         }
      }
   }

   ::u32 TightDecoder::getRawTightColor(const ::innate_subsystem::PixelFormat & pixelformat,
                                         const ::array_base<::u8> &pixels,
                                         const size_t offset)
   {
      if (m_isCPixel) {
         return pixels[offset] << 16 |
                pixels[offset + 1] << 8 |
                pixels[offset + 2];
      }
      ::u32 rawColor = 0;
      memcpy(&rawColor, &pixels[offset], pixelformat.bitsPerPixel / 8);
      return rawColor;
   }

   void TightDecoder::fillRawComponents(const ::innate_subsystem::PixelFormat & pixelformat,
                                        ::u8 components[],
                                        const ::array_base<::u8> &pixels,
                                        const size_t pixelOffset)
   {
      ::i32 rawColor = getRawTightColor(pixelformat, pixels, pixelOffset);
      components[0] = rawColor >> pixelformat.redShift & pixelformat.redMax;
      components[1] = rawColor >> pixelformat.greenShift & pixelformat.greenMax;
      components[2] = rawColor >> pixelformat.blueShift & pixelformat.blueMax;
   }
} // namespace remoting_rfb_client
