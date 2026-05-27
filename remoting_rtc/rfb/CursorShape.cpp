// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "CursorShape.h"


namespace remoting_rfb
{
   CursorShape::~CursorShape()
   {
   }

   bool CursorShape::clone(const CursorShape *srcCursorShape)
   {
      m_hotSpot = srcCursorShape->getHotSpot();
      if (m_pixels.clone(srcCursorShape->getPixels())) {
         m_mask = srcCursorShape->m_mask;
         return true;
      } else {
         return false;
      }
   }

   void CursorShape::assignMaskFromRfb(const_char_pointer srcMask)
   {
      if (!m_mask.empty()) {
         ::i32 height = m_pixels.getDimension().cy;
         size_t rfbWidthInBytes = getMaskWidthInBytes();
         size_t maskLen = height * rfbWidthInBytes;
         memcpy(m_mask.data(), srcMask, maskLen);
      }
   }

   void CursorShape::assignMaskFromWindows(const_char_pointer srcMask)
   {
      if (!m_mask.empty()) {
         ::i32 height = m_pixels.getDimension().cy;
         ::i32 winWidthInBytes = ((m_pixels.getDimension().cx + 15) / 16) * 2;
         ::i32 rfbWidthInBytes = getMaskWidthInBytes();
         for (::i32 i = 0; i < height; i++) {
            memcpy(&m_mask[i * rfbWidthInBytes],
                   &srcMask[i * winWidthInBytes],
                   rfbWidthInBytes);
         }
      }
   }

   bool CursorShape::setDimension(const ::i32_size & sizeNew)
   {
      bool result = m_pixels.setDimension(sizeNew);
      return result && resizeBuffer();
   }

   bool CursorShape::setPixelFormat(const ::innate_subsystem::PixelFormat & pixFormat)
   {
      bool result = m_pixels.setPixelFormat(pixFormat);
      return result && resizeBuffer();
   }

   bool CursorShape::setProperties(const ::i32_size & sizeNew,
                                   const ::innate_subsystem::PixelFormat & pixelFormat)
   {
      bool result = m_pixels.setDimension(sizeNew) &&
                    m_pixels.setPixelFormat(pixelFormat);
      return result && resizeBuffer();
   }

   void CursorShape::resetToEmpty()
   {
      setDimension(::i32_size(0, 0));
      setHotSpot(0, 0);
   }

   bool CursorShape::resizeBuffer()
   {
      m_mask.resize(getMaskSize());
      return true;
   }

   ::i32 CursorShape::getMaskSize() const
   {
      return getMaskWidthInBytes() * m_pixels.getDimension().cy;
   }

   ::i32 CursorShape::getMaskWidthInBytes() const
   {
      return (m_pixels.getDimension().cx + 7) / 8;
   }
} // namespace remoting_rfb