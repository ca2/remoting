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
#include "TightDecoder.h"

#include "subsystem/framebuffer/StandardPixelFormatFactory.h"

namespace remoting
{
   TightDecoder::TightDecoder(::subsystem::LogWriter * plogwriter)
   : DecoderOfRectangle(plogwriter),
     m_isCPixel(false)
   {
      m_encoding = EncodingDefs::TIGHT;

      m_inflater.resize(DECODERS_NUM);
      for (int i = 0; i < DECODERS_NUM; i++)
         m_inflater[i] = new ::subsystem::Inflater;
      reset();
   }

   TightDecoder::~TightDecoder()
   {
      for (int i = 0; i < DECODERS_NUM; i++) {
         try {
            delete m_inflater[i];
         } catch (...) {
         }
      }
   }

   void TightDecoder::decode(::remoting::RfbInputGate *pinput,
                             ::innate_subsystem::Framebuffer *pframebuffer,
                             const ::int_rectangle &  rectangleTarget)
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

      unsigned char compressionControl = pinput->readUInt8();
      resetDecoders(compressionControl);
      unsigned char compressionType = (compressionControl >> 4) & 0x0F;

      int bytesPerCPixel = pframebuffer->getBytesPerPixel();

      if (compressionType > MAX_SUBENCODING) {
         throw ::subsystem::Exception("Sub-encoding in Tight-encoder are not valid");
      }
      if (::int_rectangle(pframebuffer->getDimension()).intersection(rectangleTarget)!= rectangleTarget)
         throw ::subsystem::Exception("Error in protocol: incorrect size of rectangle (tight-decoder)");

      if (compressionType == FILL_TYPE) {
         unsigned int color = readTightPixel(pinput, bytesPerCPixel);
         pframebuffer->fillRect(rectangleTarget, color);
      } else if (compressionType == JPEG_TYPE) {
         processJpeg(pinput, pframebuffer, rectangleTarget);
      } else
         processBasicTypes(pinput, pframebuffer, rectangleTarget, compressionControl);
   }

   unsigned int TightDecoder::transformPixelToTight(unsigned int color)
   {
      unsigned int result = 0;
      result |= (color & 0xFF) << 16;
      result |= color & 0xFF00;
      result |= (color >> 16);
      return result;
   }

   ::array_base<unsigned char> TightDecoder::transformArray(const ::array_base<unsigned char> &buffer)
   {
      ::array_base<unsigned char> result(buffer.size() * 4 / 3);
      for (size_t bi = 0, ri = 0; bi < buffer.size(); bi += 3, ri += 4) {
         result[ri] = buffer[bi + 2];
         result[ri + 1] = buffer[bi + 1];
         result[ri + 2] = buffer[bi];
         result[ri + 3] = 0;
      }
      return result;
   }

   unsigned int TightDecoder::readTightPixel(::remoting::RfbInputGate *pinput, int bytesPerCPixel)
   {
      unsigned int color = 0;
      unsigned char buffer[sizeof(color)];
      if (!m_isCPixel) {
         pinput->readFully(buffer, bytesPerCPixel);
      } else {
         pinput->readFully(buffer, 3);
         unsigned char t = buffer[2];
         buffer[2] = buffer[0];
         buffer[0] = t;
         buffer[3] = 0;
      }
      memcpy(&color, buffer, bytesPerCPixel);
      return color;
   }

   void TightDecoder::reset()
   {
      for (int i = 0; i < DECODERS_NUM; i++) {
         delete m_inflater[i];
         m_inflater[i] = new ::subsystem::Inflater;
      }
   }

   void TightDecoder::resetDecoders(unsigned char compressionControl)
   {
      for (int i = 0; i < DECODERS_NUM; i++)
         if (compressionControl & (0x01 << i)) {
            delete m_inflater[i];
            m_inflater[i] = new ::subsystem::Inflater;
         }
   }

   int TightDecoder::readCompactSize(::remoting::RfbInputGate *pinput)
   {
      int b = pinput->readUInt8();
      int size = b & 0x7F;
      if ((b & 0x80) != 0) {
         b = pinput->readUInt8();
         size += (b & 0x7F) << 7;
         if ((b & 0x80) != 0) {
            size += pinput->readUInt8() << 14;
         }
      }
      return size;
   }

   void TightDecoder::processJpeg(::remoting::RfbInputGate *pinput,
                                  ::innate_subsystem::Framebuffer *pframebuffer,
                                  const ::int_rectangle &  rectangleTarget)
   {
      unsigned int jpegBufLen = readCompactSize(pinput);
      if (jpegBufLen == 0)
         throw ::subsystem::Exception("Error in protocol: empty byffer of jpeg (tight-decoder)");
      ::array_base<unsigned char> buffer;
      buffer.resize(jpegBufLen);
      pinput->readFully(buffer.data(), jpegBufLen);

      if (rectangleTarget.area() != 0) {
         ::array_base<unsigned char> pixels;
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

   void TightDecoder::processBasicTypes(::remoting::RfbInputGate *pinput,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                                        const ::int_rectangle &  rectangleTarget,
                                        unsigned char compressionControl)
   {
      int decoderId = (compressionControl & STREAM_ID_MASK) >> 4;
      int filterId = COPY_FILTER;
      if ((compressionControl & FILTER_ID_MASK) != 0) {
         filterId = pinput->readUInt8();
      }

      int bytesPerCPixel = pframebuffer->getBytesPerPixel();
      size_t lengthCurrentBpp = rectangleTarget.area() * bytesPerCPixel;
      if (m_isCPixel) {
         lengthCurrentBpp = rectangleTarget.area() * 3;
      }

      ::array_base<unsigned char> buffer;

      switch (filterId) {
         case COPY_FILTER:
            readTightData(pinput, buffer, lengthCurrentBpp, decoderId);
            if (m_isCPixel) {
               buffer = transformArray(buffer);
            }
            drawTightBytes(pframebuffer, &buffer, rectangleTarget);
            break;

            // The "gradient" filter and "jpeg" compression may be used only
            // when bits-per-pixel value is either 16 or 32, not 8.
         case PALETTE_FILTER:
         {
            int paletteSize = pinput->readUInt8() + 1;
            ::array_base<unsigned int> palette = readPalette(pinput, paletteSize, bytesPerCPixel);
            size_t dataLength = rectangleTarget.area();
            if (paletteSize == 2) {
               dataLength = (rectangleTarget.width() + 7) / 8 * rectangleTarget.height();
            }
            readTightData(pinput, buffer, dataLength, decoderId);
            drawPalette(pframebuffer, palette, buffer, rectangleTarget);
         }
            break;

         case GRADIENT_FILTER:
            readTightData(pinput, buffer, lengthCurrentBpp, decoderId);
            drawGradient(pframebuffer, buffer, rectangleTarget);
            break;

         default:
            break;
      }
   }

   ::array_base<unsigned int> TightDecoder::readPalette(::remoting::RfbInputGate *pinput,
                                         int paletteSize,
                                         int bytesPerCPixel)
   {
      ::array_base<unsigned int> palette(paletteSize);
      for (int i = 0; i < paletteSize; i++) {
         palette[i] = readTightPixel(pinput, bytesPerCPixel);
      }
      return palette;
   }

   void TightDecoder::readTightData(::remoting::RfbInputGate *pinput,
                                    ::array_base<unsigned char> &buffer,
                                    size_t expectedLength,
                                    const int decoderId)
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

   void TightDecoder::readCompressedData(::remoting::RfbInputGate *pinput,
                                         ::array_base<unsigned char> &buffer,
                                         size_t expectedLength,
                                         const int decoderId)
   {
      size_t rawDataLength = readCompactSize(pinput);

      ::array_base<char> compressed(rawDataLength);

      // read compressed (raw) data behind space allocated for decompressed data
      if (rawDataLength != 0) {
         pinput->readFully(compressed.data(), rawDataLength);

         ::subsystem::Inflater *decoder = m_inflater[decoderId];
         decoder->setInput(compressed.data(), rawDataLength);
         decoder->setUnpackedSize(expectedLength);
         decoder->inflate();

         size_t size = decoder->getOutputSize();
         const char *output = decoder->getOutput();
         buffer.resize(size);
         buffer.assign((unsigned char *) output, size);
      } else {
         _ASSERT(rawDataLength != 0);
         m_plogwriter->debug("Tight decoder: Length of Raw compressed data is 0");
         buffer.resize(0);
      }
   }

   void TightDecoder::drawPalette(::innate_subsystem::Framebuffer *pframebuffer,
                                  const ::array_base<unsigned int> &palette,
                                  const ::array_base<unsigned char> &pixels,
                                  const ::int_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (draw Tight bytes)
      int width = rectangleTarget.width();
      int height = rectangleTarget.height();

      int bytesPerPixel = pframebuffer->getBytesPerPixel();

      int dstLength = rectangleTarget.area();

      int x = rectangleTarget.left;
      int y = rectangleTarget.top;
      int stride = pframebuffer->getBytesPerRow();
      char *basePtr = (char*)pframebuffer->getBufferPtr(x, y);
      if (palette.size() == 2) {
         int offset = 8;
         int index = -1;
         for (int i = 0; i < dstLength; i++) {
            void *pixelPtr = basePtr + bytesPerPixel * (i % width) + stride * (i / width);
            if (offset == 0 || i % width == 0) {
               offset = 8;
               index++;
            }
            offset--;
            memcpy(pixelPtr, &palette[(pixels[index] >> offset) & 0x01], bytesPerPixel);
         }
      } else { // size of palette != 2
         for (int i = 0; i < dstLength; i++) {
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
                                     const ::array_base<unsigned char> *pixels,
                                     const ::int_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (zrle)
      int width = rectangleTarget.width();
      int height = rectangleTarget.height();

      int bytesPerPixel = pframebuffer->getBytesPerPixel();

      int dstLength = rectangleTarget.area();

      int x = rectangleTarget.left;
      int y = rectangleTarget.top;
      for (int i = 0; i < dstLength; i++) {
         void *pixelPtr = pframebuffer->getBufferPtr(x + i % width, y + i / width);
         memcpy(pixelPtr, &pixels->operator [](i * bytesPerPixel), bytesPerPixel);
      }
   }

   void TightDecoder::drawJpegBytes(::innate_subsystem::Framebuffer *pframebuffer,
                                    const ::array_base<unsigned char> *pixels,
                                    const ::int_rectangle &  rectangleTarget)
   {
      // TODO: removed duplicate code (draw tight bytes)
      int width = rectangleTarget.width();
      int height = rectangleTarget.height();

      int fbBytesPerPixel = pframebuffer->getBytesPerPixel();
      int bytesPerCPixel = 3;
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();

      int dstLength = rectangleTarget.area();

      int x = rectangleTarget.left;
      int y = rectangleTarget.top;
      for (int i = 0; i < dstLength; i++) {
         unsigned char color[4] = {0, 0, 0, 0};
         memcpy(&color, &pixels->operator [](i * bytesPerCPixel), bytesPerCPixel);
         unsigned int pixel = (((unsigned int)color[0] * pixelformat.redMax + 127) / 255 << pixelformat.redShift |
                        ((unsigned int)color[1] * pixelformat.greenMax + 127) / 255 << pixelformat.greenShift |
                        ((unsigned int)color[2] * pixelformat.blueMax + 127) / 255 << pixelformat.blueShift);

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
                                   const ::array_base<unsigned char> &pixels,
                                   const ::int_rectangle &  rectangleTarget)
   {
      typedef ::array_base<unsigned short> RowType;
      size_t opRowLength = rectangleTarget.width() * 3 + 3;

      ::array_base<RowType> opRows(2);
      opRows[0].resize(opRowLength);
      opRows[1].resize(opRowLength);

      memset(opRows[0].data(), 0, opRowLength * sizeof(unsigned short));
      memset(opRows[1].data(), 0, opRowLength * sizeof(unsigned short));

      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      int fbBytesPerPixel = pframebuffer->getBytesPerPixel();
      int bytesPerCPixel = fbBytesPerPixel;
      if (m_isCPixel) {
         bytesPerCPixel = 3;
      }

      int opRowIndex = 0;
      unsigned short max[3] = {pixelformat.redMax, pixelformat.greenMax, pixelformat.blueMax};
      unsigned short shift[3] = {pixelformat.redShift, pixelformat.greenShift, pixelformat.blueShift};
      size_t pixelOffset = 0;

      for (int i = 0; i < rectangleTarget.height(); ++i) {
         // exchange thisRow and prevRow:
         RowType &thisRow = opRows[opRowIndex];
         RowType &prevRow = opRows[opRowIndex = (opRowIndex + 1) % 2];

         for (size_t j = 3; j < opRowLength; j += 3, pixelOffset += bytesPerCPixel) {
            unsigned char rawColor[3];
            fillRawComponents(pixelformat, rawColor, pixels, pixelOffset);
            unsigned int color = 0;
            for (int index = 0; index < 3; index++) {
               int d = prevRow[j + index] +      // "upper" pixel (from prev row)
                         thisRow[j + index - 3] -  // prev pixel
                         prevRow[j + index - 3];   // "diagonal" prev pixel
               unsigned short converted = d < 0 ? 0 : d > max[index] ? max[index] : d;
               thisRow[j + index] = (converted + rawColor[index]) & max[index];
               color |= (thisRow[j + index] & max[index]) << shift[index];
            }
            void *pixelPtr = pframebuffer->getBufferPtr(static_cast<int>(rectangleTarget.left + j/3 - 1),
                                              rectangleTarget.top + i);
            memcpy(pixelPtr, &color, fbBytesPerPixel);
         }
      }
   }

   unsigned int TightDecoder::getRawTightColor(const ::innate_subsystem::PixelFormat & pixelformat,
                                         const ::array_base<unsigned char> &pixels,
                                         const size_t offset)
   {
      if (m_isCPixel) {
         return pixels[offset] << 16 |
                pixels[offset + 1] << 8 |
                pixels[offset + 2];
      }
      unsigned int rawColor = 0;
      memcpy(&rawColor, &pixels[offset], pixelformat.bitsPerPixel / 8);
      return rawColor;
   }

   void TightDecoder::fillRawComponents(const ::innate_subsystem::PixelFormat & pixelformat,
                                        unsigned char components[],
                                        const ::array_base<unsigned char> &pixels,
                                        const size_t pixelOffset)
   {
      int rawColor = getRawTightColor(pixelformat, pixels, pixelOffset);
      components[0] = rawColor >> pixelformat.redShift & pixelformat.redMax;
      components[1] = rawColor >> pixelformat.greenShift & pixelformat.greenMax;
      components[2] = rawColor >> pixelformat.blueShift & pixelformat.blueMax;
   }
} // namespace remoting