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
#include "CursorPainter.h"

//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_client
{
   CursorPainter::CursorPainter(::innate_subsystem::Framebuffer *pframebuffer, ::subsystem::LogWriter * plogwriter)
   : m_fb(pframebuffer),
     m_plogwriter(plogwriter),
     m_cursorIsMoveable(false),
     m_ignoreShapeUpdates(false),
     m_isExist(false)
   {
   }

   CursorPainter::~CursorPainter()
   {
   }

   void CursorPainter::updatePointerPos(const ::i32_point &pointPosition)
   {
      critical_section_lock al(&m_lock);
      m_pointerPosition = pointPosition;
      m_cursorIsMoveable = true;

      // Now, cursor is ready for painting.
   }

   void CursorPainter::setNewCursor(const ::i32_point &pointHotspot,
                                    ::u16 width, ::u16 height,
                                    const ::array_base<::u8> *cursor,
                                    const ::array_base<::u8> *bitmask)
   {
      critical_section_lock al(&m_lock);
      m_plogwriter->information("setNewCursor Cursor hot-spot is ({}, {})", pointHotspot.x, pointHotspot.y);
      m_cursor.setHotSpot(pointHotspot.x, pointHotspot.y);

      m_plogwriter->information("setNewCursor Cursor size is ({}, {})", width, height);
      ::i32_size cursorDimension(width, height);
      ::innate_subsystem::PixelFormat pixelFormat = m_fb->getPixelFormat();

      m_cursor.setProperties(cursorDimension, pixelFormat);
      m_cursorOverlay.setProperties(cursorDimension, pixelFormat);

      size_t pixelSize = m_fb->getBytesPerPixel();
      size_t cursorSize = width * height * pixelSize;
      // Server is allowed to specify zero as width and/or height of the cursor.
      //if (0)
      {
         if (cursorSize != 0) {
            m_plogwriter->debug("Set image of cursor...");
            memcpy(m_cursor.getPixels()->getBuffer(), cursor->data(), cursorSize);
            m_plogwriter->debug("Set bitmask of cursor...");
            m_cursor.assignMaskFromRfb(reinterpret_cast<const_char_pointer >(bitmask->data()));
         }
      }
   }

   void CursorPainter::setIgnoreShapeUpdates(bool ignore)
   {
      m_plogwriter->debug("Set flag of ignor by cursor update is '{}'", ignore);

      critical_section_lock al(&m_lock);
      m_ignoreShapeUpdates = ignore;
   }

   ::i32_rectangle CursorPainter::hideCursor()
   {
      critical_section_lock al(&m_lock);

      if (!m_isExist) {
         return ::i32_rectangle();
      }

      m_isExist = false;

      ::i32_rectangle erase(m_cursorOverlay.getDimension());
      ::i32_point corner = getUpperLeftPoint(m_pointLastPosition);

      erase.offset(corner.x, corner.y);

      m_plogwriter->debug("Cursor rectangle: ({}, {}), ({}, {})", erase.left, erase.top, erase.right, erase.bottom);

      if (erase.area() == 0) {
         return ::i32_rectangle();
      }

      m_plogwriter->debug("Erasing cursor...");
      m_fb->copyFrom(erase, &m_cursorOverlay, 0, 0);

      return erase;
   }

   ::i32_rectangle CursorPainter::showCursor()
   {
      critical_section_lock al(&m_lock);

      m_pointLastPosition = m_pointerPosition;

      if (m_isExist) {
         m_plogwriter->error("Error in CursorPainter: painting ::f64 copy of cursor.");
         ASSERT(true);
      }

      if (!m_bHideCursor && !m_ignoreShapeUpdates && m_cursorIsMoveable && m_cursor.getDimension().area() != 0) {

         m_plogwriter->debug("Painting cursor...");

         ::i32_point corner = getUpperLeftPoint(m_pointLastPosition);

         m_cursorOverlay.copyFrom(m_fb, corner.x, corner.y);

         ::i32_rectangle overlayRect(m_cursor.getDimension());
         overlayRect.offset(corner.x, corner.y);
         //overlayRect /= m_fb->m_iDivisor;

         m_fb->overlay(overlayRect, m_cursor.getPixels(), 0, 0, m_cursor.getMask());

         m_isExist = true;
         return overlayRect;
      }
      return ::i32_rectangle();
   }

   ::i32_point CursorPainter::getUpperLeftPoint(const ::i32_point &pointPosition) const
   {
      ::i32_point upperLeftPoint = pointPosition;
      upperLeftPoint-= m_cursor.getHotSpot();
      return upperLeftPoint;
   }
} //namespace remoting_client
