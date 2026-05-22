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


#include "remoting/remoting_rtc_windows/_common_header.h"
#include "remoting/remoting_rtc/desktop/ScreenDriver.h"
#include "remoting/remoting_rtc_windows/desktop/WinVideoRegionUpdaterImpl.h"
#include "remoting/remoting_rtc/desktop/CursorPositionDetector.h"
#include "remoting/remoting_rtc/desktop/CursorShapeDetector.h"
#include "remoting/remoting_rtc_windows/desktop/WindowsCursorShapeGrabber.h"
#include "remoting/remoting_rtc/desktop/CopyRectDetector.h"


namespace remoting_rtc_windows
{

   // This class  implements "grabbers" and "detectors" which is not couple� with screen frame buffer.
   class CLASS_DECL_REMOTING_RTC_WINDOWS Win32ScreenDriverBaseImpl :
      virtual public WinVideoRegionUpdaterImpl
   {
   public:
      //Win32ScreenDriverBaseImpl(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
        //                        lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);


      Win32ScreenDriverBaseImpl();
      ~Win32ScreenDriverBaseImpl() override;


      void initialize_screen_driver(::remoting_rtc_node::Configurator * pconfigurator, ::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;


      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual bool grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat);
      virtual const ::remoting_rtc::CursorShape *getCursorShape();
      virtual ::i32_point getCursorPosition();

      virtual void getCopiedRegion(::i32_rectangle &rectangleCopy, ::i32_point & pointSource);

   protected:
      lockable_critical_section *framebuffer_critical_section();

   private:
      lockable_critical_section *m_pcriticalsectionFramebuffer;

      ::pointer < ::remoting_rtc::CursorPositionDetector >  m_pcursorpositiondetector;
      ::pointer < WindowsCursorShapeGrabber > m_pcursorshapegrabber;
      ::pointer < ::remoting_rtc::CursorShapeDetector > m_pcursorshapedetector;

      ::pointer < ::remoting_rtc::CopyRectDetector > m_pcopyrectdetector;
   };

   //// __WIN32SCREENDRIVERBASEIMPL_H__


} // namespace remoting_rtc_windows
 


