// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting_rtc/rfb_sconn/EncodeOptions.h"
#include "remoting/remoting_rtc/rfb/CursorShape.h"
#include "remoting/remoting_rtc/desktop/UpdateContainer.h"
//#include "subsystem/platform/class ::time.h"
//#include "subsystem/thread/lockable_critical_section.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING  CursorUpdates :
   virtual public ::particle
   {
   public:

      // Current state
      ::i32_point m_cursorPos;
      class ::time m_blockCurPosTime;
      ::remoting_rtc::CursorShape m_cursorshape;
      ::innate_subsystem::Framebuffer m_shapeBackground;
      ::i32_point m_pointBackground;
      lockable_critical_section m_criticalsectionCurPosLoc;
      // Uses when the rich enabled but pointer pos disabled to determine
      // the last send method: by a cursor shape update or drawing on the
      // frame buffer.
      bool m_isDrawCursorMethod;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;



      CursorUpdates();
      virtual ~CursorUpdates();


      virtual void initialize_cursor_updates(::subsystem::LogWriter * plogwriter);

      // Important: After calling the update() function frame buffer
      // may contain a cursor image to send it as a frame buffer update.
      // After sending the frame buffer updates the user code must at sight
      // call the restoreFramebuffer() function. Also function clones
      // actual cursor shape to the cursorShape argument (Only when after call
      // the updatecontainer.m_bCursorShapeChanged flag is raised).
      void update(const EncodeOptions *encodeOptions, UpdateContainer & updatecontainer, bool fullRegReq,
                  const ::i32_rectangle &rectangleViewport, bool shareOnlyApp, const ::remoting_rtc::Region & regionShareApp,
                  ::innate_subsystem::Framebuffer *pframebuffer, ::remoting_rtc::CursorShape *cursorShape);
      void restoreFramebuffer(::innate_subsystem::Framebuffer *pframebuffer);

      // Returns current cursor pointPosition. Beetween
      ::i32_point getCurPos();

      // Returns background rectangle.
      ::i32_rectangle getBackgroundRect();

      // Block cursor pos sending by this
      // connection to a client. Unblocking will
      // be automaticly for a time.
      void blockCursorPosSending();

      // Clones (updates internal) the cursor shape.
      void updateCursorShape(const ::remoting_rtc::CursorShape *curShape);

   //private:
      // Clones the internal cursor shape to curShape.
      void extractCursorShape(::remoting_rtc::CursorShape *curShape);

      // Check cursor pointPosition for changing and store it to the m_cursorPos.
      // Return true value if cursor pointPosition has been changed.
      bool checkCursorPos(UpdateContainer & updatecontainer, const ::i32_rectangle &rectangleViewport, bool curPosBlockingIsIgnored);

      // Shortcut function to draw cursor on the frame buffer directly.
      void drawCursor(UpdateContainer & updatecontainer, ::innate_subsystem::Framebuffer *pframebuffer);

      // Check for cursor blocking state and
      // return true if it is blocked and false
      // otherwise.
      bool isCursorPosBlocked();

   };


} // namespace remoting







