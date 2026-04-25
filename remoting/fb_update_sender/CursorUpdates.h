// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting/rfb_sconn/EncodeOptions.h"
#include "remoting/remoting/rfb/CursorShape.h"
#include "remoting/remoting/desktop/UpdateContainer.h"
//#include "subsystem/platform/::earth::time.h"
//#include "subsystem/thread/critical_section.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING  CursorUpdates
   {
   public:
      CursorUpdates();
      virtual ~CursorUpdates();


      virtual void initialize_cursor_updates(::subsystem::LogWriter * plogwriter);

      // Important: After calling the update() function frame buffer
      // may contain a cursor image to send it as a frame buffer update.
      // After sending the frame buffer updates the user code must at sight
      // call the restoreFrameBuffer() function. Also function clones
      // actual cursor shape to the cursorShape argument (Only when after call
      // the updCont->cursorShapeChanged flag is raised).
      void update(const EncodeOptions *encodeOptions, UpdateContainer *pupdatecontainer, bool fullRegReq,
                  const ::int_rectangle &viewPort, bool shareOnlyApp, const ::remoting::Region *shareAppRegion,
                  ::innate_subsystem::FrameBuffer *fb, ::remoting::CursorShape *cursorShape);
      void restoreFrameBuffer(::innate_subsystem::FrameBuffer *fb);

      // Returns current cursor position. Beetween
      ::int_point getCurPos();

      // Returns background rectangle.
      ::int_rectangle getBackgroundRect();

      // Block cursor pos sending by this
      // connection to a client. Unblocking will
      // be automaticly for a time.
      void blockCursorPosSending();

      // Clones (updates internal) the cursor shape.
      void updateCursorShape(const ::remoting::CursorShape *curShape);

   private:
      // Clones the internal cursor shape to curShape.
      void extractCursorShape(::remoting::CursorShape *curShape);

      // Check cursor position for changing and store it to the m_cursorPos.
      // Return true value if cursor position has been changed.
      bool checkCursorPos(UpdateContainer *updCont, const ::int_rectangle &viewPort, bool curPosBlockingIsIgnored);

      // Shortcut function to draw cursor on the frame buffer directly.
      void drawCursor(UpdateContainer *updCont, ::innate_subsystem::FrameBuffer *fb);

      // Check for cursor blocking state and
      // return true if it is blocked and false
      // otherwise.
      bool isCursorPosBlocked();

      // Current state
      ::int_point m_cursorPos;
      class ::time m_blockCurPosTime;
      ::remoting::CursorShape m_cursorShape;
      ::innate_subsystem::FrameBuffer m_shapeBackground;
      ::int_point m_pointBackground;
      critical_section m_curPosLocMut;
      // Uses when the rich enabled but pointer pos disabled to determine
      // the last send method: by a cursor shape update or drawing on the
      // frame buffer.
      bool m_isDrawCursorMethod;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting







