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

#pragma once


//#include "log_writer/LogWriter.h"
#include "remoting/remoting_common/rfb/CursorShape.h"
//#include "subsystem/thread/critical_section.h"

namespace remoting
{
   class CLASS_DECL_REMOTING_COMMON CursorPainter
   {
   public:
      CursorPainter(::subsystem_apex::FrameBuffer *fb, ::subsystem::LogWriter *logWriter);
      virtual ~CursorPainter();

      // this functions is thread-safe for private data of cursor, but need external lock of frame buffer
      ::int_rectangle hideCursor();
      ::int_rectangle showCursor();

      // this functions is thread-safe
      void setIgnoreShapeUpdates(bool ignore);
      void updatePointerPos(const ::int_point *position);
      void setNewCursor(const ::int_point *hotSpot,
                        unsigned short width, unsigned short height,
                        const ::array_base<unsigned char> *cursor,
                        const ::array_base<unsigned char> *bitmask);
      //private:
      // This function is thread-save.
      ::int_point getUpperLeftPoint(const ::int_point *position) const;

      ::subsystem::LogWriter *m_plogwriter;

      ::subsystem_apex::FrameBuffer *const m_fb;

      critical_section m_lock;
      CursorShape m_cursor;

      // Actual position of pointer
      ::int_point m_pointerPosition;

      // Last painted position of pointer
      ::int_point m_lastPosition;
      // Copy of rect frame buffer under cursor
      ::subsystem_apex::FrameBuffer m_cursorOverlay;

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
} // namespace remoting