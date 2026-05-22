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

#pragma once

#include "remoting/remoting_rfb_windows/_common_header.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"
#include <DXGI1_2.h>
//#include aaa_<vector>


namespace remoting_rfb_windows
{

   // This class  provides some functions to work with windows cursor shape data.
   class CLASS_DECL_REMOTING_RFB_WINDOWS WinCursorShapeUtils
   {
   public:
      WinCursorShapeUtils();
      virtual ~WinCursorShapeUtils();


      // Builds rfb mask AND by alpha channel of given pixels.
      // If isInversedAlpha is false the pixel supposes by transparent if alpha value less than 128.
      // If isInversedAlpha is true the pixel supposes by transparent if alpha value greater than 128.
      static void fixAlphaChannel(const ::innate_subsystem::Framebuffer *pixels, char *maskAND, bool isInversedAlpha,
                                  int maskWidthInBytes);

      // This function combines windows cursor mask and image and convert
      // theirs to rfb format. This function uses for monochrome cursor image.
      static void winMonoShapeToRfb(const ::innate_subsystem::Framebuffer *pixels, char *maskAND, char *maskXOR,
                                    int maskWidthInBytes);

      //   This function combines windows the cursor mask and image and convert
      // theirs to rfb format. This function uses for 16 or 24 bit color cursor
      // image.
      //   Also, this function determines whether image contains the alhpa channel
      // and returns true in this case.
      template<typename T>
      static bool winColorShapeToRfb(const ::innate_subsystem::Framebuffer *pixels, char *maskAND,
                                     int maskWidthInBytes);

      // Matrox videocard returns 256 byte width buffer for 32 pixel cursor,
      // need trim it for correct handling
      static void trimBuffer(::array_base<char> *buffer, DXGI_OUTDUPL_POINTER_SHAPE_INFO *shapeInfo);

   private:
      // Inverts bit array with the "not" operator.
      static void inverse(char *bits, int count);

      // Returns true if a bit on the index place is true. High-order bit has zero index.
      inline static bool testBit(char byte, int index)
      {
         char dummy = 128 >> index;
         return (dummy & byte) != 0;
      };

      inline static char setBit(char byte, int index)
      {
         char dummy = 128 >> index;
         return dummy | byte;
      };

      inline static char clearBit(char byte, int index)
      {
         char dummy = 128 >> index;
         return ~dummy & byte;
      };

      static ::u32 getAlphaMask(const ::innate_subsystem::PixelFormat & pixelformat);
      static ::u32 getCursorHeight(DXGI_OUTDUPL_POINTER_SHAPE_INFO &shapeInfo);
      static bool isPixelTransparent(char *const buffer, ::u32 type, ::u32 height, ::u32 pitch,
                                     ::u32 x, ::u32 y);
      static bool isColorPixelTransparent(::u32 pixel, ::u32 type);
      static bool isMonochromePixelTransparent(char andByte, char xorByte, ::u32 x);
      static void trimTransparent(::array_base<char> *buffer, DXGI_OUTDUPL_POINTER_SHAPE_INFO *shapeInfo);
   };

   template<typename T>
   static bool WinCursorShapeUtils::winColorShapeToRfb(const ::innate_subsystem::Framebuffer *pixels, char *maskAND,
                                                       int maskWidthInBytes)
   {
      char *pixelsBuffer = (char *)pixels->getBuffer();
      ::innate_subsystem::PixelFormat pixelformat = pixels->getPixelFormat();
      T *pixel;

      int fbWidth = pixels->getDimension().cx;
      int fbHeight = pixels->getDimension().cy;

      bool hasAlphaChannel = false;
      ::u32 alphaMask = getAlphaMask(pixelformat);

      for (int iRow = 0; iRow < fbHeight; iRow++)
      {
         for (int iCol = 0; iCol < fbWidth; iCol++)
         {
            pixel = (T *)pixelsBuffer + iRow * fbWidth + iCol;

            size_t iMaskAnd = iRow * maskWidthInBytes + iCol / 8;
            bool maskANDBit = testBit(maskAND[iMaskAnd], iCol % 8);

            if (!maskANDBit)
            { // *pixel = *pixel
               // Set current mask bit to true
               maskAND[iMaskAnd] = setBit(maskAND[iMaskAnd], iCol % 8);
            }
            else if ((*pixel >> pixelformat.redShift & pixelformat.redMax) == 0 && (*pixel >> pixelformat.greenShift & pixelformat.greenMax) == 0 &&
                     (*pixel >> pixelformat.blueShift & pixelformat.blueMax) == 0)
            { // Transparent dot
               maskAND[iMaskAnd] = clearBit(maskAND[iMaskAnd], iCol % 8);
            }
            else
            { // Inverted (as black dot)
               // Set current mask bit to true
               maskAND[iMaskAnd] = setBit(maskAND[iMaskAnd], iCol % 8);
               // Set pixel to black with the alpa channel preserving
               *pixel &= ~((T)pixelformat.redMax << pixelformat.redShift);
               *pixel &= ~((T)pixelformat.greenMax << pixelformat.greenShift);
               *pixel &= ~((T)pixelformat.blueMax << pixelformat.blueShift);
            }
            // Test for the alpha channel presence
            hasAlphaChannel = hasAlphaChannel || (*pixel & alphaMask) != 0;
         }
      }
      return hasAlphaChannel;
   }

   //// __WINCURSORSHAPEUTILS_H__

} // namespace remoting_rfb_windows





