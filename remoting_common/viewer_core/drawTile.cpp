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
#include "ZrleDecoder.h"

#include "subsystem_acme/io/ByteArrayInputStream.h"

//#include aaa_<vector>
//#include aaa_<algorithm>

namespace remoting
{
   void ZrleDecoder::drawTile(::subsystem_apex::FrameBuffer* fb,
       const ::int_rectangle& tileRect,
       const ::array_base<char>* pixels)
   {
      int width = tileRect.width();
      size_t fbBytesPerPixel = m_bytesPerPixel;

      if (fbBytesPerPixel == 3) {
         fbBytesPerPixel = 4;
      }

      int tileLength = tileRect.area();

      int x = tileRect.left;
      int y = tileRect.top;

      const char* pixelsPtr = pixels->data();
      char* bufferPtr = 0;
      int h = tileLength / width;

      if (fbBytesPerPixel == 4 && m_bytesPerPixel == 3)
      {

         auto ptr = (char*)fb->getBuffer();
         auto bytesPerPixel = fb->getBytesPerPixel();
         if (bytesPerPixel == 3)
         {
            //void* getBufferPtr(int x, int y) const
            //{
            //    char* ptr = (char*)m_buffer;
            //    ptr += (y * m_dimension.cx + x) * getBytesPerPixel();

            //    return (void*)ptr;
            //}

            for (int j = 0; j < h; j++) {
               auto p = ptr + ((y + j) * fb->m_dimension.cx + x) * 3;
               for (int i = 0; i < width; i++) {

                  bufferPtr = (char*)p;
                  bufferPtr[0] = pixelsPtr[0];
                  bufferPtr[1] = pixelsPtr[1];
                  bufferPtr[2] = pixelsPtr[2];
                  bufferPtr[3] = 0;
                  pixelsPtr += 3;
                  p += 3;
               }
            }
         }
         else if (bytesPerPixel == 4)
         {

            if (width == fb->m_dimension.cx and x == 0)
            {

               const int count = width * h;
               uint8_t* dst = (uint8_t * )ptr + y * fb->m_dimension.cx * 4 + x * 4;
               const uint8_t* src = (const uint8_t * )pixelsPtr;

               for (int i = 0; i < count; ++i) {
                  *reinterpret_cast<uint32_t*>(dst) =
                      (uint32_t(src[0])) |
                      (uint32_t(src[1]) << 8) |
                      (uint32_t(src[2]) << 16);

                  src += 3;
                  dst += 4;
               }
            }
            else
            {
               //auto cx4 = fb->m_dimension.cx * 4;
               //auto p1 = ptr + (y + 0) * cx4 + (x * 4);
               //for (int j = 0; j < h; j++) {
               //    auto p = p1;
               //    for (int i = 0; i < width; i++) {

               //        p[0] = pixelsPtr[0];
               //        p[1] = pixelsPtr[1];
               //        p[2] = pixelsPtr[2];
               //        p[3] = 0;
               //        pixelsPtr += 3;
               //        p += 4;
               //    }
               //    p1 += cx4;
               //}

               const int dstStride = fb->m_dimension.cx * 4;
               uint8_t* dstRow = (uint8_t *) ptr + y * dstStride + x * 4;
               const uint8_t* __restrict src = (const uint8_t * ) pixelsPtr;

               for (int j = 0; j < h; ++j) {
                  uint8_t* __restrict dst = dstRow;

                  for (int i = 0; i < width; ++i) {
                     *reinterpret_cast<uint32_t*>(dst) =
                         (uint32_t(src[0])) |
                         (uint32_t(src[1]) << 8) |
                         (uint32_t(src[2]) << 16);

                     src += 3;
                     dst += 4;
                  }

                  dstRow += dstStride;
               }

            }

         }
         else
         {
            for (int j = 0; j < h; j++) {
               auto p = ptr + ((y + j) * fb->m_dimension.cx + x) * bytesPerPixel;
               for (int i = 0; i < width; i++) {

                  bufferPtr = (char*)p;
                  bufferPtr[0] = pixelsPtr[0];
                  bufferPtr[1] = pixelsPtr[1];
                  bufferPtr[2] = pixelsPtr[2];
                  bufferPtr[3] = 0;
                  pixelsPtr += 3;
                  p += bytesPerPixel;
               }
            }

         }
      }
      else
      {

         for (int j = 0; j < h; j++) {
            for (int i = 0; i < width; i++) {

               bufferPtr = (char*)fb->getBufferPtr(x + i, y + j);
               memset(bufferPtr, 0, fbBytesPerPixel);
               memcpy(bufferPtr, pixelsPtr, m_bytesPerPixel);
               pixelsPtr += m_bytesPerPixel;
            }
         }

      }
   }
} // namespace remoting