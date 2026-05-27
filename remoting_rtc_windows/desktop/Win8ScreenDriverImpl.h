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


//#include "log_writer/LogWriter.h"
#include "subsystem_windows/thread/GuiThread.h"
#include "subsystem_windows/thread/ThreadCollector.h"
#include "acme/parallelization/happening.h"

#include "innate_subsystem/framebuffer/Framebuffer.h"
#include "remoting/remoting_rtc_windows/desktop/Win8CursorShape.h"
#include "remoting/remoting_rtc/desktop/UpdateKeeper.h"
#include "remoting/remoting_rtc/desktop/UpdateListener.h"
#include "remoting/remoting_rtc_windows/desktop/Win8DuplicationListener.h"
#include "remoting/remoting_rtc_windows/desktop/Win8DeskDuplicationThread.h"


namespace remoting_rtc_windows
{


   class CLASS_DECL_REMOTING_RTC_WINDOWS Win8ScreenDriverImpl :
      virtual public ::subsystem_windows::GuiThread,
      virtual public Win8DuplicationListener
   {
   public:


      ::pointer<D3D11Device> m_pd3d11device;
      
      //Win8ScreenDriverImpl(::subsystem::LogWriter * plogwriter, ::remoting_rtc::UpdateKeeper *pupdatekeeper,
        //                   lockable_critical_section *pcriticalsectionFramebuffer, ::remoting_rtc::UpdateListener *pupdatelistener,
          //                 bool detectionEnabled = false);
      Win8ScreenDriverImpl();
      ~Win8ScreenDriverImpl() override;

      virtual void initialize_win8_screen_driver_impl(::subsystem::LogWriter * plogwriter, ::remoting_rtc::UpdateKeeper *pupdatekeeper,
                           lockable_critical_section *pcriticalsectionFramebuffer, ::remoting_rtc::UpdateListener *pupdatelistener,
                           bool detectionEnabled = false);

      void executeDetection();
      void terminateDetection();

      bool grabFb(const ::i32_rectangle & rectangle);

      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();

      // Updates destination (*dst) cursor shape properties and data.
      void updateCursorShape(::remoting_rtc::CursorShape *dst);
      ::i32_point getCursorPosition();

      bool isValid();

   protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   private:
      // Implementions of the Win8DuplicationListener listener functions.
      virtual void onFramebufferUpdate(const ::remoting_rtc::Region & regionChanged);
      virtual void onCopyRect(const ::i32_rectangle &rectangleTarget, ::i32 srcX, ::i32 srcY);
      virtual void onCursorPositionChanged(::i32 x, ::i32 y);
      virtual void onCursorShapeChanged();
      virtual void onRecoverableError(const ::scoped_string &scopedstrReason);
      virtual void onCriticalError(const ::scoped_string &scopedstrReason);

      void initDxgi();

      // This function always return the DX DXGI_FORMAT_B8G8R8A8_UNORM format in the ::innate_subsystem::PixelFormat
      // type.
      ::innate_subsystem::PixelFormat getDxPixelFormat() const;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      //::subsystem_windows::ThreadCollector m_deskDuplThreadBundle;

      ::pointer<Win8DeskDuplication> m_pdeskduplication;

      ::happening m_happeningInit;
      ::happening m_happeningError;

      // The duplication interface can't be used
      bool m_hasCriticalError;
      // The interface can be used but it should be reinitialized.
      bool m_hasRecoverableError;

      // The frame buffer with appropriate properties creates once at the constructor time. And then
      // has these properties permanently.
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebufferProperty;

      // Cursor's properties changes at all time. And then it should be safe by a local mutex.
      ::i32_point m_latestCursorPos;
      Win8CursorShape m_win8CursorShape;
      LONGLONG m_curTimeStamp;
      lockable_critical_section m_cursorMutex;

      ::pointer < ::remoting_rtc::UpdateKeeper > m_pupdatekeeper;
      ::pointer < ::remoting_rtc::UpdateListener > m_pupdatelistener;
      bool m_detectionEnabled;
   };

   //// __WIN8SCREENDRIVERIMPL_H__


} // namespace remoting_rtc_windows






