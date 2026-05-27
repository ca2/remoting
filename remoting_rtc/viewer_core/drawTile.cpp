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
#include "ZrleDecoder.h"

#include "acme/input_output/ByteArrayInputStream.h"

//#include aaa_<vector>
//#include aaa_<algorithm>

namespace remoting_client
{
   void ZrleDecoder::drawTile(::innate_subsystem::Framebuffer* pframebuffer,
       const ::i32_rectangle& tileRect,
       const ::array_base<::i8>* pixels)
   {
      ::i32 width = tileRect.width();
      size_t fbBytesPerPixel = m_bytesPerPixel;

      if (fbBytesPerPixel == 3) {
         fbBytesPerPixel = 4;
      }

      ::i32 tileLength = tileRect.area();

      ::i32 x = tileRect.left;
      ::i32 y = tileRect.top;

      const_char_pointer pixelsPtr = pixels->data();
      char_pointer bufferPtr = 0;
      ::i32 h = tileLength / width;

      if (fbBytesPerPixel == 4 && m_bytesPerPixel == 3)
      {

         auto ptr = (char_pointer )pframebuffer->getBuffer();
         auto bytesPerPixel = pframebuffer->getBytesPerPixel();
         if (bytesPerPixel == 3)
         {
            //void* getBufferPtr(::i32 x, ::i32 y) const
            //{
            //    char_pointer ptr = (char_pointer )m_buffer;
            //    ptr += (y * m_size.cx + x) * getBytesPerPixel();

            //    return (void*)ptr;
            //}

            for (::i32 j = 0; j < h; j++) {
               auto p = ptr + ((y + j) * pframebuffer->m_size.cx + x) * 3;
               for (::i32 i = 0; i < width; i++) {

                  bufferPtr = (char_pointer )p;
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

            if (width == pframebuffer->m_size.cx and x == 0)
            {

               const ::i32 count = width * h;
               uint8_t* dst = (uint8_t * )ptr + y * pframebuffer->m_size.cx * 4 + x * 4;
               const uint8_t* src = (const uint8_t * )pixelsPtr;

               for (::i32 i = 0; i < count; ++i) {
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
               //auto cx4 = pframebuffer->m_size.cx * 4;
               //auto p1 = ptr + (y + 0) * cx4 + (x * 4);
               //for (::i32 j = 0; j < h; j++) {
               //    auto p = p1;
               //    for (::i32 i = 0; i < width; i++) {

               //        p[0] = pixelsPtr[0];
               //        p[1] = pixelsPtr[1];
               //        p[2] = pixelsPtr[2];
               //        p[3] = 0;
               //        pixelsPtr += 3;
               //        p += 4;
               //    }
               //    p1 += cx4;
               //}

               const ::i32 dstStride = pframebuffer->m_size.cx * 4;
               uint8_t* dstRow = (uint8_t *) ptr + y * dstStride + x * 4;
               const uint8_t* __restrict src = (const uint8_t * ) pixelsPtr;

               for (::i32 j = 0; j < h; ++j) {
                  uint8_t* __restrict dst = dstRow;

                  for (::i32 i = 0; i < width; ++i) {
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
            for (::i32 j = 0; j < h; j++) {
               auto p = ptr + ((y + j) * pframebuffer->m_size.cx + x) * bytesPerPixel;
               for (::i32 i = 0; i < width; i++) {

                  bufferPtr = (char_pointer )p;
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

         for (::i32 j = 0; j < h; j++) {
            for (::i32 i = 0; i < width; i++) {

               bufferPtr = (char_pointer )pframebuffer->getBufferPtr(x + i, y + j);
               memset(bufferPtr, 0, fbBytesPerPixel);
               memcpy(bufferPtr, pixelsPtr, m_bytesPerPixel);
               pixelsPtr += m_bytesPerPixel;
            }
         }

      }
   }
} // namespace remoting_client