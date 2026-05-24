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

#pragma once


//#include "log_writer/LogWriter.h"
#include "remoting/remoting_rfb/rfb/CursorShape.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_rfb_client
{
   class CLASS_DECL_REMOTING_RDX CursorPainter
   {
   public:
      CursorPainter(::innate_subsystem::Framebuffer *pframebuffer, ::subsystem::LogWriter * plogwriter);
      virtual ~CursorPainter();

      // this functions is thread-safe for private data of cursor, but need external lock of frame buffer
      ::i32_rectangle hideCursor();
      ::i32_rectangle showCursor();

      // this functions is thread-safe
      void setIgnoreShapeUpdates(bool ignore);
      void updatePointerPos(const ::i32_point &pointPosition);
      void setNewCursor(const ::i32_point &pointHotspot,
                        unsigned short width, unsigned short height,
                        const ::array_base<unsigned char> *cursor,
                        const ::array_base<unsigned char> *bitmask);
      //private:
      // This function is thread-save.
      ::i32_point getUpperLeftPoint(const ::i32_point &pointPosition) const;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      ::innate_subsystem::Framebuffer *const m_fb;

      lockable_critical_section m_lock;
      ::remoting_rfb::CursorShape m_cursor;

      // Actual pointPosition of pointer
      ::i32_point m_pointerPosition;

      // Last painted pointPosition of pointer
      ::i32_point m_pointLastPosition;
      // Copy of rectangle frame buffer under cursor
      ::innate_subsystem::Framebuffer m_cursorOverlay;

      // Flag is set, if cursor is showed.
      bool m_isExist;

      // Flag is set after first call updatePointerPosition().
      // If flag is unset then pointer isn't painted.
      bool m_cursorIsMoveable;

      bool m_ignoreShapeUpdates;
      bool m_bHideCursor = false;

   private:
      // Do not allow copying objects.
      CursorPainter(const CursorPainter &);
      CursorPainter &operator=(const CursorPainter &);
   };
} // namespace remoting_rfb_client