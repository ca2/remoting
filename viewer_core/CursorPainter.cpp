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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "CursorPainter.h"

#include "thread/AutoLock.h"

CursorPainter::CursorPainter(FrameBuffer *fb, LogWriter *logWriter)
: m_fb(fb),
  m_logWriter(logWriter),
  m_cursorIsMoveable(false),
  m_ignoreShapeUpdates(false),
  m_isExist(false)
{
}

CursorPainter::~CursorPainter()
{
}

void CursorPainter::updatePointerPos(const Point *position)
{
  AutoLock al(&m_lock);
  m_pointerPosition = *position;
  m_cursorIsMoveable = true;

  // Now, cursor is ready for painting.
}

void CursorPainter::setNewCursor(const Point *hotSpot,
                                 unsigned short width, unsigned short height,
                                 const ::array_base<unsigned char> *cursor,
                                 const ::array_base<unsigned char> *bitmask)
{
  AutoLock al(&m_lock);
  m_logWriter->debug("Cursor hot-spot is ({}, {})", hotSpot->x, hotSpot->y);
  m_cursor.setHotSpot(hotSpot->x, hotSpot->y);

  m_logWriter->debug("Cursor size is ({}, {})", width, height);
  ::int_size cursorDimension(width, height);
  PixelFormat pixelFormat = m_fb->getPixelFormat();

  m_cursor.setProperties(cursorDimension, pixelFormat);
  m_cursorOverlay.setProperties(cursorDimension, pixelFormat);

  size_t pixelSize = m_fb->getBytesPerPixel();
  size_t cursorSize = width * height * pixelSize;
  // Server is allowed to specify zero as width and/or height of the cursor.
   //if (0)
   {
      if (cursorSize != 0) {
         m_logWriter->debug("Set image of cursor...");
         memcpy(m_cursor.getPixels()->getBuffer(), cursor->data(), cursorSize);
         m_logWriter->debug("Set bitmask of cursor...");
         m_cursor.assignMaskFromRfb(reinterpret_cast<const char *>(bitmask->data()));
      }
   }
}

void CursorPainter::setIgnoreShapeUpdates(bool ignore)
{
  m_logWriter->debug("Set flag of ignor by cursor update is '{}'", ignore);

  AutoLock al(&m_lock);
  m_ignoreShapeUpdates = ignore;
}

::int_rectangle CursorPainter::hideCursor()
{
  AutoLock al(&m_lock);

  if (!m_isExist) {
    return ::int_rectangle();
  }

  m_isExist = false;

  ::int_rectangle erase(m_cursorOverlay.getDimension());
  Point corner = getUpperLeftPoint(&m_lastPosition);

  erase.offset(corner.x, corner.y);

  m_logWriter->information("Cursor rect: ({}, {}), ({}, {})", erase.left, erase.top, erase.right, erase.bottom);

  if (erase.area() == 0) {
    return ::int_rectangle();
  }

  m_logWriter->debug("Erasing cursor...");
  m_fb->copyFrom(erase, &m_cursorOverlay, 0, 0);

  return erase;
}

::int_rectangle CursorPainter::showCursor()
{
  AutoLock al(&m_lock);

  m_lastPosition = m_pointerPosition;

  if (m_isExist) {
    m_logWriter->error("Error in CursorPainter: painting double copy of cursor.");
    _ASSERT(true);
  }

  if (!m_ignoreShapeUpdates && m_cursorIsMoveable && m_cursor.getDimension().area() != 0) {

    m_logWriter->debug("Painting cursor...");

    Point corner = getUpperLeftPoint(&m_lastPosition);

    m_cursorOverlay.copyFrom(m_fb, corner.x, corner.y);

    ::int_rectangle overlayRect(m_cursor.getDimension());
    overlayRect.offset(corner.x, corner.y);

    m_fb->overlay(overlayRect, m_cursor.getPixels(), 0, 0, m_cursor.getMask());

    m_isExist = true;
    return overlayRect;
  }
  return ::int_rectangle();
}

Point CursorPainter::getUpperLeftPoint(const Point *position) const
{
  Point upperLeftPoint = *position;
  upperLeftPoint.move(-m_cursor.getHotSpot().x, -m_cursor.getHotSpot().y);
  return upperLeftPoint;
}
