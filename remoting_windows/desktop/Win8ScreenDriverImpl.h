// Copyright (C) 2012 GlavSoft LLC.
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
#include "subsystem/thread/GuiThread.h"
#include "subsystem/thread/ThreadCollector.h"
#include "acme/parallelization/happening.h"

#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_windows/desktop/Win8CursorShape.h"
#include "remoting/remoting/desktop/UpdateKeeper.h"
#include "remoting/remoting/desktop/UpdateListener.h"
#include "remoting/remoting_windows/desktop/Win8DuplicationListener.h"
#include "remoting/remoting_windows/desktop/Win8DeskDuplicationThread.h"


namespace remoting
{


   class CLASS_DECL_REMOTING_WINDOWS Win8ScreenDriverImpl : private GuiThread, private Win8DuplicationListener
   {
   public:
      Win8ScreenDriverImpl(::subsystem::LogWriter * plogwriter, UpdateKeeper *pupdatekeeper,
                           lockable_critical_section *pcriticalsectionFramebuffer, UpdateListener *pupdatelistener,
                           bool detectionEnabled = false);
      virtual ~Win8ScreenDriverImpl();

      void executeDetection();
      void terminateDetection();

      bool grabFb(const ::int_rectangle & rectangle);

      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();

      // Updates destination (*dst) cursor shape properties and data.
      void updateCursorShape(CursorShape *dst);
      ::int_point getCursorPosition();

      bool isValid();

   protected:
      virtual void execute();
      virtual void onTerminate();

   private:
      // Implementions of the Win8DuplicationListener listener functions.
      virtual void onFramebufferUpdate(const Region & regionChanged);
      virtual void onCopyRect(const ::int_rectangle &rectangleTarget, int srcX, int srcY);
      virtual void onCursorPositionChanged(int x, int y);
      virtual void onCursorShapeChanged();
      virtual void onRecoverableError(const ::scoped_string &scopedstrReason);
      virtual void onCriticalError(const ::scoped_string &scopedstrReason);

      void initDxgi();

      // This function always return the DX DXGI_FORMAT_B8G8R8A8_UNORM format in the ::innate_subsystem::PixelFormat
      // type.
      ::innate_subsystem::PixelFormat getDxPixelFormat() const;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      ThreadCollector m_deskDuplThreadBundle;

      ::happening m_initEvent;
      ::happening m_errorEvent;

      // The duplication interface can't be used
      bool m_hasCriticalError;
      // The interface can be used but it should be reinitialized.
      bool m_hasRecoverableError;

      // The frame buffer with appropriate properties creates once at the constructor time. And then
      // has these properties permanently.
      ::innate_subsystem::Framebuffer m_pframebuffer;

      // Cursor's properties changes at all time. And then it should be safe by a local mutex.
      ::int_point m_latestCursorPos;
      Win8CursorShape m_win8CursorShape;
      LONGLONG m_curTimeStamp;
      lockable_critical_section m_cursorMutex;

      UpdateKeeper *m_pupdatekeeper;
      UpdateListener *m_pupdatelistener;
      bool m_detectionEnabled;
   };

   //// __WIN8SCREENDRIVERIMPL_H__


} // namespace remoting






