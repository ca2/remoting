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

#include "::remoting_rtc::ScreenDriver.h"
//#include "log_writer/LogWriter.h"
#include "::remoting_rtc::UpdateKeeper.h"
#include "::remoting_rtc::UpdateListener.h"
#include "subsystem/thread/Thread.h"
#include "acme/parallelization/happening.h"


namespace remoting_windows
{

   class CLASS_DECL_REMOTING DummyScreenDriver : public ::remoting_rtc::ScreenDriver, ::subsystem::Thread
   {
   public:
      //DummyScreenDriver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener, const ::i32_size & size,
        //                unsigned int interval, ::subsystem::LogWriter * plogwriter);

      DummyScreenDriver();
      virtual ~DummyScreenDriver();

      virtual void initialize_dummy_screen_driver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener, const ::i32_size & size,
                        unsigned int interval, ::subsystem::LogWriter * plogwriter);
      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual ::i32_size getScreenDimension();
      virtual bool grabFb(const ::i32_rectangle & rectangle = {});
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool getScreenPropertiesChanged();
      virtual bool getScreenSizeChanged();
      virtual bool applyNewScreenProperties();
      bool grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat) { return true; };
      const CursorShape *getCursorShape() { return &m_cursorshape; };
      ::i32_point getCursorPosition() { return ::i32_point(); };

      void getCopiedRegion(::i32_rectangle &rectangleCopy, ::i32_point & pointSource) { return; };
      Region getVideoRegion() { return Region(); };

   protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   private:
      ::pointer < ::innate_subsystem::Framebuffer > m_pframebufferWork;
      CursorShape m_cursorshape;
      ::pointer < ::remoting_rtc::UpdateKeeper  > m_pupdatekeeper;
      ::pointer < ::remoting_rtc::UpdateListener  > m_pupdatelistener;
      ::happening m_happeningSleeper;
      unsigned int m_interval;
      bool m_detectionEnabled;
   };


} // namespace remoting_windows
 



