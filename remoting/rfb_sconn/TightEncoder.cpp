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
#include "TightEncoder.h"

#include "acme/input_output/ByteArrayOutputStream.h"

namespace remoting
{
   TightEncoder::TightEncoder(PixelConverter * ppixelconverter, DataOutputStream * pdataoutputstream)
   : Encoder(ppixelconverter, pdataoutputstream)
   {
      for (int i = 0; i < NUM_ZLIB_STREAMS; i++) {
         m_zsActive[i] = false;
      }
   }

   TightEncoder::~TightEncoder()
   {
      for (int i = 0; i < NUM_ZLIB_STREAMS; i++) {
         if (m_zsActive[i]) {
            deflateEnd(&m_zsStruct[i]);
         }
      }
   }

   int TightEncoder::getCode() const
   {
      return EncodingDefs::TIGHT;
   }

   void TightEncoder::splitRectangle(const ::int_rectangle &  rectangle,
                                     ::int_rectangle_array_base & rectanglea,
                                     const ::innate_subsystem::Framebuffer *serverFb,
                                     const EncodeOptions *options)
   {
      int maxSize = getConf(options).maxRectSize;
      int rectWidth = rectangle.width();

      // Handle a special case -- don't ever split a rectangle while its area does
      // not exceed 1/4 of maximum allowed area and width does not exceed 2048.
      if (rectWidth <= 2048 && rectangle.area() <= maxSize / 4) {
         rectanglea.add(rectangle);
         return;
      }

      int maxWidth = getConf(options).maxRectWidth;
      if (maxWidth > rectWidth) {
         maxWidth = rectWidth;
      }

      int maxHeight = maxSize / maxWidth;

      for (int y0 = rectangle.top; y0 < rectangle.bottom; y0 += maxHeight) {
         int y1 = minimum(y0 + maxHeight, rectangle.bottom);
         for (int x0 = rectangle.left; x0 < rectangle.right; x0 += maxWidth) {
            int x1 = minimum(x0 + maxWidth, rectangle.right);
            rectanglea.add(::int_rectangle(x0, y0, x1, y1));
         }
      }
   }

   void TightEncoder::sendRectangle(const ::int_rectangle &  rectangle,
                                    const ::innate_subsystem::Framebuffer *serverFb,
                                    const EncodeOptions *options)
   {
      // First, convert pixels to client format.
      const ::innate_subsystem::Framebuffer *clientFb = m_ppixelconverter->convert(rectangle, serverFb);

      // Now call an encoder function corresponding to the client's pixel size.
      size_t bpp = clientFb->getBitsPerPixel();
      switch (bpp) {
         case 8:
            sendAnyRect<unsigned char>(rectangle, serverFb, clientFb, options);
            break;
         case 16:
            sendAnyRect<unsigned short>(rectangle, serverFb, clientFb, options);
            break;
         case 32:
            sendAnyRect<unsigned int>(rectangle, serverFb, clientFb, options);
            break;
         default:
            ASSERT(0);
      }
   }

   //--------------------------------------------------------------------------//

   // FIXME: Is it really necessary to pass both frame buffers in arguments?
   // FIXME: Make a special version for the case when PIXEL_T is unsigned char.
   template <class PIXEL_T>
   void TightEncoder::sendAnyRect(const ::int_rectangle &  rectangle,
                                  const ::innate_subsystem::Framebuffer *serverFb,
                                  const ::innate_subsystem::Framebuffer *clientFb,
                                  const EncodeOptions *options)
   {
      // Compute maximum number of colors to be allowed in the palette.
      int maxColors = rectangle.area() / getConf(options).idxMaxColorsDivisor;
      if (maxColors < 2) {
         if (rectangle.area() >= getConf(options).monoMinRectSize) {
            maxColors = 2;
         } else {
            // Always allow at least one color in the palette.
            maxColors = 1;
         }
      }

      // Fill in the palette (m_pal).
      fillPalette<PIXEL_T>(rectangle, clientFb, maxColors);

      // If that was a solid-color rectangle, sent it.
      int numColors = m_pal.getNumColors();
      if (numColors == 1) {
         sendSolidRect(rectangle, clientFb);
      } else if (numColors == 2) {
         sendMonoRect<PIXEL_T>(rectangle, clientFb, options);
      } else if (sizeof(PIXEL_T) > 1 && numColors != 0) {
         sendIndexedRect<PIXEL_T>(rectangle, clientFb, options);
      } else if (sizeof(PIXEL_T) > 1 &&
                 options->jpegEnabled() &&
                 serverFb->getBitsPerPixel() >= 16 &&
                 rectangle.area() >= JPEG_MIN_RECT_SIZE &&
                 rectangle.width() >= JPEG_MIN_RECT_WIDTH &&
                 rectangle.height() >= JPEG_MIN_RECT_HEIGHT) {
         sendJpegRect(rectangle, serverFb, options);
                 } else {
                    sendFullColorRect<PIXEL_T>(rectangle, clientFb, options);
                 }
   }

   void TightEncoder::sendSolidRect(const ::int_rectangle &  r, const ::innate_subsystem::Framebuffer *pframebuffer)
   {
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      size_t pixelSize = pixelformat.bitsPerPixel / 8;

      // Copy the leftmost upper pixel of the rectangle into a buffer.
      unsigned char buf[4];
      memcpy(buf, pframebuffer->getBufferPtr(r.left, r.top), pixelSize);
      // FIXME: Call packPixels() unconditionally, make it return length in bytes?
      if (shouldPackPixels(pixelformat)) {
         packPixels(buf, 1, pixelformat);
         pixelSize = 3;
      }

      m_pdataoutputstream->writeUInt8(SUBENCODING_FILL);
      m_pdataoutputstream->write(buf, pixelSize);
   }

   template <class PIXEL_T>
   void TightEncoder::sendMonoRect(const ::int_rectangle &  rectangle,
                                   const ::innate_subsystem::Framebuffer *pframebuffer,
                                   const EncodeOptions *options)
   {
      // Send control info.
      const int zlibStreamId = ZLIB_STREAM_MONO;
      m_pdataoutputstream->writeUInt8(EXPLICIT_FILTER | zlibStreamId << 4);
      m_pdataoutputstream->writeUInt8(FILTER_PALETTE);
      m_pdataoutputstream->writeUInt8(1); // the number of colors minus 1

      // Prepare output buffer.
      int dataLen = (rectangle.width() + 7) / 8;
      dataLen *= rectangle.height();
      // FIXME: Optimize, use char[] instead of ByteArrayOutputStream.
      ByteArrayOutputStream encoded(this, dataLen);
      //encoded.initialize_byte_array_output_stream(dataLen);
      DataOutputStream encodedData(&encoded);
      //encodedData.initialize_data_output_stream(&encoded);

      // Send the palette.
      PIXEL_T palette[2] = {
         (PIXEL_T)m_pal.getEntry(0),
         (PIXEL_T)m_pal.getEntry(1)
       };
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      size_t pixelSize = sizeof(PIXEL_T);
      if (shouldPackPixels(pixelformat)) {
         packPixels((unsigned char *)palette, 2, pixelformat);
         pixelSize = 3;
      }
      m_pdataoutputstream->write(palette, pixelSize * 2);

      // Convert image to indexed colors.
      encodeMonoRect<PIXEL_T>(rectangle, pframebuffer, &encodedData);
      ASSERT(encoded.size() == dataLen);

      // Compress and send.
      int zlibLevel = getConf(options).monoZlibLevel;
      sendCompressed(encoded.toByteArray(), dataLen, zlibStreamId, zlibLevel);
   }

   template <class PIXEL_T>
   void TightEncoder::sendIndexedRect(const ::int_rectangle &  rectangle,
                                      const ::innate_subsystem::Framebuffer *pframebuffer,
                                      const EncodeOptions *options)
   {
      // Send control info.
      const int zlibStreamId = ZLIB_STREAM_IDX;
      m_pdataoutputstream->writeUInt8(EXPLICIT_FILTER | zlibStreamId << 4);
      m_pdataoutputstream->writeUInt8(FILTER_PALETTE);
      int numColors = m_pal.getNumColors();
      m_pdataoutputstream->writeUInt8((unsigned char)(numColors - 1));

      // Prepare output buffer.
      int dataLen = rectangle.width() * rectangle.height();
      // FIXME: Optimize, use char[] instead of ByteArrayOutputStream.
      ByteArrayOutputStream encoded(this, dataLen);
      //encoded.initialize_byte_array_output_stream(dataLen);
      DataOutputStream encodedData(&encoded);
      //encodedData.initialize_data_output_stream(&encoded);

      // Send the palette.
      PIXEL_T palette[256];
      for (int i = 0; i < numColors; i++) {
         palette[i] = (PIXEL_T)m_pal.getEntry(i);
      }
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      size_t pixelSize = sizeof(PIXEL_T);
      if (shouldPackPixels(pixelformat)) {
         packPixels((unsigned char *)palette, numColors, pixelformat);
         pixelSize = 3;
      }
      m_pdataoutputstream->write(palette, pixelSize * numColors);

      // Convert image to indexed colors.
      encodeIndexedRect<PIXEL_T>(rectangle, pframebuffer, &encodedData);
      ASSERT(encoded.size() == dataLen);

      // Compress and send.
      int zlibLevel = getConf(options).idxZlibLevel;
      sendCompressed(encoded.toByteArray(), dataLen, zlibStreamId, zlibLevel);
   }

   template <class PIXEL_T>
   void TightEncoder::sendFullColorRect(const ::int_rectangle &  rectangle,
                                        const ::innate_subsystem::Framebuffer *pframebuffer,
                                        const EncodeOptions *options)
   {
      // Send control info.
      const int zlibStreamId = ZLIB_STREAM_RAW;
      m_pdataoutputstream->writeUInt8(zlibStreamId << 4);

      // Prepare output buffer.
      int dataLen = rectangle.area() * sizeof(PIXEL_T);
      // FIXME: Use char[] instead?
      ::array_base<unsigned char> rgbData(dataLen);

      // Get pixels from the frame buffer.
      copyPixels<PIXEL_T>(rectangle, pframebuffer, rgbData.data());
      ASSERT(rgbData.size() == dataLen);

      // Pack pixels into 24-bit samples if necessary.
      ::innate_subsystem::PixelFormat pixelformat = pframebuffer->getPixelFormat();
      if (shouldPackPixels(pixelformat)) {
         packPixels(rgbData.data(), rectangle.area(), pixelformat);
         rgbData.resize(rectangle.area() * 3);
      }

      // Compress and send.
      int zlibLevel = getConf(options).rawZlibLevel;
      // FIXME: Get rid of explicit conversions between chars and bytes.
      sendCompressed((const char *)rgbData.data(), rgbData.size(),
                     zlibStreamId, zlibLevel);
   }

   void TightEncoder::sendJpegRect(const ::int_rectangle &  rectangle,
                                   const ::innate_subsystem::Framebuffer *serverFb,
                                   const EncodeOptions *options)
   {
      ASSERT(options->jpegEnabled());

      // Set proper JPEG quality level in the compressor. The default value 6
      // below does not mean anything, it will not be used because we assume
      // valid JPEG quality level was set in the options object.
      int quality = options->getJpegQualityLevel(6);
      m_compressor.setQuality(quality * 10 + 5);

      // Shortcuts.
      const void *ptr = serverFb->getBufferPtr(rectangle.left, rectangle.top);
      ::innate_subsystem::PixelFormat fmt = serverFb->getPixelFormat();
      int width = rectangle.width();
      int height = rectangle.height();
      int stride = serverFb->getBytesPerRow();

      // Compress pixels.
      m_compressor.compress(ptr, fmt, width, height, stride);
      size_t dataLength = m_compressor.getOutputLength();

      // Actually send the encoded data.
      m_pdataoutputstream->writeUInt8(SUBENCODING_JPEG);
      sendCompactLength(dataLength);
      m_pdataoutputstream->write(m_compressor.getOutputData(), dataLength);
   }

   //--------------------------------------------------------------------------//

   bool TightEncoder::shouldPackPixels(const ::innate_subsystem::PixelFormat & pixelformat) const
   {
      return (pixelformat.colorDepth == 24 &&
              pixelformat.redMax == 0xFF &&
              pixelformat.greenMax == 0xFF &&
              pixelformat.blueMax == 0xFF &&
              pixelformat.bitsPerPixel == 32);
   }

   void TightEncoder::packPixels(unsigned char *buf, int count, const ::innate_subsystem::PixelFormat & pixelformat)
   {
      unsigned char *dst = buf;
      unsigned int pix;

      while (count--) {
         if (!pixelformat.bigEndian) {
            pix = (unsigned int)buf[3] << 24 |
                  (unsigned int)buf[2] << 16 |
                  (unsigned int)buf[1] << 8 |
                  (unsigned int)buf[0];
         } else {
            pix = (unsigned int)buf[0] << 24 |
                  (unsigned int)buf[1] << 16 |
                  (unsigned int)buf[2] << 8 |
                  (unsigned int)buf[3];
         }
         buf += 4;
         *dst++ = (unsigned char)(pix >> pixelformat.redShift);
         *dst++ = (unsigned char)(pix >> pixelformat.greenShift);
         *dst++ = (unsigned char)(pix >> pixelformat.blueShift);
      }
   }

   template <class PIXEL_T>
   void TightEncoder::fillPalette(const ::int_rectangle &  r, const ::innate_subsystem::Framebuffer *pframebuffer, int maxColors)
   {
      // Clear the palette.
      m_pal.reset();
      m_pal.setMaxColors(maxColors);

      // Shortcuts.
      const PIXEL_T *pixels = (const PIXEL_T *)pframebuffer->getBuffer();
      int stride = pframebuffer->getDimension().cx;
      unsigned int pixel = pixels[r.top * stride + r.left];
      unsigned int oldPixel = 0;
      unsigned int runLength = 0;

      for (int y = r.top; y < r.bottom; y++) {
         for (int x = r.left; x < r.right; x++) {
            pixel = pixels[y * stride + x];

            if (oldPixel != pixel) {
               if (m_pal.insert(oldPixel, runLength) == 0) {
                  return;
               }
               oldPixel = pixel;
               runLength = 1;
            } else {
               runLength++;
            }
         }
      }
      if (m_pal.insert(oldPixel, runLength) == 0) {
         return;
      }
   }

   template <class PIXEL_T>
   void TightEncoder::copyPixels(const ::int_rectangle &  rectangle, const ::innate_subsystem::Framebuffer *pframebuffer,
                                 unsigned char *dst)
   {
      const int rectWidth = rectangle.width();
      const int rectHeight = rectangle.height();

      const PIXEL_T *src = (const PIXEL_T *)pframebuffer->getBufferPtr(rectangle.left, rectangle.top);
      const int fbStride = pframebuffer->getDimension().cx;
      const int bytesPerRow = rectWidth * sizeof(PIXEL_T);

      for (int y = 0; y < rectHeight; y++) {
         memcpy(dst, src, bytesPerRow);
         src += fbStride;
         dst += bytesPerRow;
      }
   }

   template <class PIXEL_T>
   void TightEncoder::encodeMonoRect(const ::int_rectangle &  rectangle, const ::innate_subsystem::Framebuffer *pframebuffer,
                                     DataOutputStream *out)
   {
      const PIXEL_T *src = (const PIXEL_T *)pframebuffer->getBufferPtr(rectangle.left, rectangle.top);
      const int w = rectangle.width();
      const int h = rectangle.height();
      const PIXEL_T bg = (PIXEL_T)m_pal.getEntry(0);

      unsigned int value, mask;
      int x, y, bits;
      const int alignedWidth = w - w % 8;
      const int skipPixels = pframebuffer->getDimension().cx - w;

      for (y = 0; y < h; y++) {
         for (x = 0; x < alignedWidth; x += 8) {
            for (bits = 0; bits < 8; bits++) {
               if (*src++ != bg)
                  break;
            }
            if (bits == 8) {
               out->writeUInt8(0);
               continue;
            }
            mask = 0x80 >> bits;
            value = mask;
            for (bits++; bits < 8; bits++) {
               mask >>= 1;
               if (*src++ != bg) {
                  value |= mask;
               }
            }
            out->writeUInt8((unsigned char)value);
         }
         if (x < w) {
            mask = 0x80;
            value = 0;
            do {
               if (*src++ != bg) {
                  value |= mask;
               }
               mask >>= 1;
            } while (++x < w);
            out->writeUInt8((unsigned char)value);
         }
         src += skipPixels;
      }
   }

   template <class PIXEL_T>
   void TightEncoder::encodeIndexedRect(const ::int_rectangle &  rectangle, const ::innate_subsystem::Framebuffer *pframebuffer,
                                        DataOutputStream *out)
   {
      const PIXEL_T *src = (const PIXEL_T *)pframebuffer->getBufferPtr(rectangle.left, rectangle.top);
      const int w = rectangle.width();
      const int h = rectangle.height();
      const int skipPixels = pframebuffer->getDimension().cx - w;

      unsigned char index = m_pal.getIndex(*src);
      PIXEL_T oldColor = 0;
      for (int y = 0; y < h; y++) {
         for (int x = 0; x < w; x++) {
            if (oldColor != *src) {
               index = m_pal.getIndex(*src);
               oldColor = *src;
            }
            *src++;
            out->writeUInt8(index);
         }
         src += skipPixels;
      }
   }

   void TightEncoder::sendCompressed(const char *data, size_t dataLen,
                                     int streamId, int zlibLevel)
   {
      if (dataLen < TIGHT_MIN_TO_COMPRESS) {
         m_pdataoutputstream->write(data, dataLen);
         return;
      }

      z_streamp pz = &m_zsStruct[streamId];

      // Initialize compression stream if needed.
      if (!m_zsActive[streamId]) {
         pz->zalloc = Z_NULL;
         pz->zfree = Z_NULL;
         pz->opaque = Z_NULL;

         int err = deflateInit2(pz, zlibLevel, Z_DEFLATED, MAX_WBITS,
                                MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
         if (err != Z_OK) {
            throw ::io_exception(error_io, "Zlib stream initialization failed in Tight encoder");
         }

         m_zsActive[streamId] = true;
         m_zsLevel[streamId] = zlibLevel;
      }

      // Prepare buffers.
      size_t compressedBufferSize = dataLen + dataLen / 100 + 16;

      ::array_base<char> charBuff;
      charBuff.set_size(compressedBufferSize);
      char *compressedData = charBuff.data();

      ASSERT((unsigned int)dataLen == dataLen);
      ASSERT((unsigned int)compressedBufferSize == compressedBufferSize);
      pz->next_in = (Bytef *)data;
      pz->avail_in = (unsigned int)dataLen;
      pz->next_out = (Bytef *)compressedData;
      pz->avail_out = (unsigned int)compressedBufferSize;

      // Change compression parameters if needed.
      if (zlibLevel != m_zsLevel[streamId]) {
         int err = deflateParams(pz, zlibLevel, Z_DEFAULT_STRATEGY);
         if (err != Z_OK) {
            throw ::io_exception(error_io, "Error configuring Zlib stream in Tight encoder");
         }
         m_zsLevel[streamId] = zlibLevel;
      }

      // Actual compression.
      int err = deflate(pz, Z_SYNC_FLUSH);
      if (err != Z_OK || pz->avail_in != 0 || pz->avail_out == 0) {
         throw ::io_exception(error_io, "Zlib compression failed in Tight encoder");
      }

      try {
         size_t compressedLength = compressedBufferSize - pz->avail_out;
         sendCompactLength(compressedLength);
         m_pdataoutputstream->write(compressedData, compressedLength);
      } catch (...) {
         throw;
      }
   }

   void TightEncoder::sendCompactLength(size_t dataLen)
   {
      ASSERT(dataLen <= 0x3FFFFF);

      unsigned char buffer[4];
      size_t numBytes = 0;

      buffer[numBytes++] = dataLen & 0x7F;
      if (dataLen > 0x7F) {
         buffer[numBytes-1] |= 0x80;
         buffer[numBytes++] = dataLen >> 7 & 0x7F;
         if (dataLen > 0x3FFF) {
            buffer[numBytes-1] |= 0x80;
            buffer[numBytes++] = dataLen >> 14 & 0xFF;
         }
      }

      m_pdataoutputstream->write(buffer, numBytes);
   }

   // FIXME: Values for maxRectSize and maxRectWidth should be determined after
   //        running TightEncoder on recorded test sessions. Current values were
   //        intentionally made small because we do not implement algorithms to
   //        detect areas to be compressed with JPEG yet and thus we would like
   //        to divide areas to avoid compressing too much with JPEG.
   const TightEncoder::Conf TightEncoder::m_conf[10] = {
      {   512,   32,   6, 0, 0, 0,  4 },
      {  2048,   64,   6, 1, 1, 1,  8 },
      {  6144,  128,   8, 3, 3, 2, 24 },
      {  8192,  128,  12, 5, 5, 3, 32 },
      {  8192,  128,  12, 6, 6, 4, 32 },
      {  8192,  128,  12, 7, 7, 5, 32 },
      {  8192,  128,  16, 7, 7, 6, 48 },
      { 16384,  256,  16, 8, 8, 7, 64 },
      { 16384,  256,  32, 9, 9, 8, 64 },
      { 32768,  256,  32, 9, 9, 9, 96 }
   };

   const TightEncoder::Conf &
   TightEncoder::getConf(const EncodeOptions *options)
   {
      int level = options->getCompressionLevel(DEFAULT_COMPRESSION_LEVEL);
      ASSERT(level >= 0 && level <= 9);
      return m_conf[level];
   }
} // namespace remoting


