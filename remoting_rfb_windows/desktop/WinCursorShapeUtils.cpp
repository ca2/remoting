// Copyright (C) 2012 GlavSoft LLC.
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
#include "remoting/remoting_rfb_windows/desktop/WinCursorShapeUtils.h"

namespace remoting_rfb_windows
{


   WinCursorShapeUtils::WinCursorShapeUtils() {}

   WinCursorShapeUtils::~WinCursorShapeUtils() {}

   void WinCursorShapeUtils::winMonoShapeToRfb(const ::innate_subsystem::Framebuffer *pixels, char *maskAND,
                                               char *maskXOR, int maskWidthInBytes)
   {
      char *pixelsBuffer = (char *)pixels->getBuffer();
      char *pixel;
      int pixelSize = pixels->getBytesPerPixel();
      int pixelCount = pixels->getBufferSize() / pixelSize;

      int fbWidth = pixels->getDimension().cx;
      int fbHeight = pixels->getDimension().cy;

      for (int iRow = 0; iRow < fbHeight; iRow++)
      {
         for (int iCol = 0; iCol < fbWidth; iCol++)
         {
            pixel = pixelsBuffer + (iRow * fbWidth + iCol) * pixelSize;

            char byteAnd = maskAND[iRow * maskWidthInBytes + iCol / 8];
            char byteXor = maskXOR[iRow * maskWidthInBytes + iCol / 8];

            bool maskANDBit = testBit(byteAnd, iCol % 8);
            bool maskXORBit = testBit(byteXor, iCol % 8);

            if (!maskANDBit && !maskXORBit)
            { // Black dot
               memset(pixel, 0, pixelSize);
            }
            else if (!maskANDBit && maskXORBit)
            { // White dot
               memset(pixel, 0xff, pixelSize);
            }
            else if (maskANDBit && maskXORBit)
            { // Inverted (as black dot)
               memset(pixel, 0, pixelSize);
            }
         }
      }

      inverse(maskAND, maskWidthInBytes * fbHeight);
      for (int i = 0; i < maskWidthInBytes * fbHeight; i++)
      {
         *(maskAND + i) |= *(maskXOR + i);
      }
   }

   void WinCursorShapeUtils::fixAlphaChannel(const ::innate_subsystem::Framebuffer *pixels, char *maskAND,
                                             bool maskedColor, int maskWidthInBytes)
   {
      ::innate_subsystem::PixelFormat pixelformat = pixels->getPixelFormat();
      if (pixelformat.bitsPerPixel != 32)
      {
         return;
      }
      ::u32 alphaMask = getAlphaMask(pixelformat);
      unsigned short alphaShift = 0;

      for (; alphaShift < 32 && ((alphaMask >> alphaShift) % 2) == 0; alphaShift++)
      {
      }

      if (alphaShift > 24)
      { // we don't know how to handle it
         return;
      }

      ::u32 *pixelBuffer = (::u32 *)pixels->getBuffer();
      int pixelSize = pixels->getBytesPerPixel();

      int fbWidth = pixels->getDimension().cx;
      int fbHeight = pixels->getDimension().cy;

      for (int iRow = 0; iRow < fbHeight; iRow++)
      {
         for (int iCol = 0; iCol < fbWidth; iCol++)
         {
            ::u32 *pixel = &pixelBuffer[iRow * fbWidth + iCol];
            ::u32 colorValue = *pixel & ~alphaMask;
            ::u32 alpha = (*pixel & alphaMask) >> alphaShift;
            bool transparent = (alpha < 128);
            if (maskedColor)
            {
               transparent = !transparent;
            }
            if (transparent)
            {
               char *byteAnd = &maskAND[iRow * maskWidthInBytes + iCol / 8];
               *byteAnd = clearBit(*byteAnd, iCol % 8);
            }
         }
      }
   }


   ::u32 WinCursorShapeUtils::getAlphaMask(const ::innate_subsystem::PixelFormat & pixelformat)
   {
      if (pixelformat.bitsPerPixel == 32)
      {
         ::u32 alphaMax = pixelformat.redMax << pixelformat.redShift | pixelformat.greenMax << pixelformat.greenShift | pixelformat.blueMax << pixelformat.blueShift;
         return ~alphaMax;
      }
      else
      {
         return 0;
      }
   }

   void WinCursorShapeUtils::inverse(char *bits, int count)
   {
      for (int i = 0; i < count; i++, bits++)
      {
         *bits = ~*bits;
      }
   }

   void WinCursorShapeUtils::trimBuffer(::array_base<char> *buffer, DXGI_OUTDUPL_POINTER_SHAPE_INFO *shapeInfo)
   {
      ::u32 newPitch;
      ::u32 oldPitch = shapeInfo->Pitch;
      trimTransparent(buffer, shapeInfo);
      if (shapeInfo->Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         newPitch = ((shapeInfo->Width + 15) / 16) * 2;
      }
      else
      {
         newPitch = shapeInfo->Width * 4;
      }

      if (newPitch >= oldPitch)
      {
         return;
      }

      for (int i = 1; i < shapeInfo->Height; i++)
      {
         char *dst = &buffer->at(i * newPitch);
         char *src = &buffer->at(i * oldPitch);
         memcpy(dst, src, newPitch);
      }
      shapeInfo->Pitch = newPitch;
   }

   void WinCursorShapeUtils::trimTransparent(::array_base<char> *buffer, DXGI_OUTDUPL_POINTER_SHAPE_INFO *shapeInfo)
   {
      ::u32 pitch = shapeInfo->Pitch;
      ::u32 height = getCursorHeight(*shapeInfo);
      ::u32 width = shapeInfo->Width;
      ::u32 hotspotX = (::u32)(shapeInfo->HotSpot.x);
      ::u32 type = shapeInfo->Type;

      // width
      const ::u32 minimumWidth = 16;
      ::u32 trimmedWidth = minimumWidth;

      for (::u32 y = 0; y < height; ++y)
      {
         for (::u32 x = width - 1; x > trimmedWidth; --x)
         {
            if (!isPixelTransparent(buffer->data(), type, height, pitch, x, y))
            {
               trimmedWidth = x + 1;
            }
         }
      }

      // Force to be on a 2-byte word boundary
      trimmedWidth = ((trimmedWidth + 0xF) & ~0xF);

      if (trimmedWidth < shapeInfo->Width)
      {
         shapeInfo->Width = trimmedWidth;
      }

      // height
      ::u32 trimmedHeight = minimumWidth;

      for (::u32 x = 0; x < width; ++x)
      {
         for (::u32 y = height - 1; y > trimmedHeight; --y)
         {
            if (!isPixelTransparent(buffer->data(), type, height, pitch, x, y))
            {
               trimmedHeight = y + 1;
            }
         }
      }
      if (trimmedHeight != height)
      {
         if (type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
         {
            shapeInfo->Height = (2 * trimmedHeight);

            // Shift the XOR mask
            char *dst = &buffer->at(trimmedHeight * pitch);
            char *src = &buffer->at(height * pitch);
            memcpy(dst, src, trimmedHeight * pitch);
         }
         else
         {
            shapeInfo->Height = trimmedHeight;
         }
      }
   }

   bool WinCursorShapeUtils::isMonochromePixelTransparent(char andByte, char xorByte, ::u32 x)
   {
      bool pixelSet = WinCursorShapeUtils::testBit(andByte, x % 8);
      bool xorSet = WinCursorShapeUtils::testBit(xorByte, x % 8);
      ;
      bool transparent = (pixelSet && !xorSet);

      return transparent;
   }

   bool WinCursorShapeUtils::isColorPixelTransparent(::u32 pixel, ::u32 type)
   {
      bool transparent;
      // color data is 32 bpp ARGB DIB
      const ::u32 alphaMask = 0xFF000000;
      const ::u32 transparencyThreshold = 0x800000;
      const ::u32 colorMask = 0x00FFFFFF;

      if (type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR)
      {
         transparent = ((pixel & alphaMask) < transparencyThreshold);
      }
      else
      {
         ::u32 color = (pixel & colorMask);
         bool xorSet = ((pixel & alphaMask) != 0); // XOR value can only be 0x00 (overwrite) or 0xFF (transparent)
         transparent = (xorSet && (color == 0));
      }

      return transparent;
   }

   bool WinCursorShapeUtils::isPixelTransparent(char *const buffer, ::u32 type, ::u32 height,
                                                ::u32 pitch, ::u32 x, ::u32 y)
   {
      if (type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         ::u32 andOffset = (y * pitch) + (x / 8);
         ::u32 xorOffset = andOffset + height * pitch;
         char andByte = buffer[andOffset];
         char xorByte = buffer[xorOffset];

         return isMonochromePixelTransparent(andByte, xorByte, x);
      }

      ::u32 offset = (y * pitch) + (x * 4);
      ::u32 *pixel = (::u32 *)(&buffer[offset]);

      return isColorPixelTransparent(*pixel, type);
   }

   ::u32 WinCursorShapeUtils::getCursorHeight(DXGI_OUTDUPL_POINTER_SHAPE_INFO &shapeInfo)
   {
      if (shapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
      {
         return shapeInfo.Height /= 2;
      }

      return shapeInfo.Height;
   }

} // namespace remoting_rfb_windows
