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


#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting/desktop/ScreenDriver.h"
#include "remoting/remoting_windows/desktop/WinVideoRegionUpdaterImpl.h"
#include "remoting/remoting/desktop/CursorPositionDetector.h"
#include "remoting/remoting/desktop/CursorShapeDetector.h"
#include "remoting/remoting_windows/desktop/WindowsCursorShapeGrabber.h"
#include "remoting/remoting/desktop/CopyRectDetector.h"


namespace remoting_windows
{

   // This class  implements "grabbers" and "detectors" which is not couple� with screen frame buffer.
   class CLASS_DECL_REMOTING_WINDOWS Win32ScreenDriverBaseImpl :
      virtual public WinVideoRegionUpdaterImpl
   {
   public:
      //Win32ScreenDriverBaseImpl(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
        //                        lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);


      Win32ScreenDriverBaseImpl();
      ~Win32ScreenDriverBaseImpl() override;


      void initialize_screen_driver(::remoting::Configurator * pconfigurator, ::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;


      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual bool grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat);
      virtual const ::remoting::CursorShape *getCursorShape();
      virtual ::int_point getCursorPosition();

      virtual void getCopiedRegion(::int_rectangle &rectangleCopy, ::int_point & pointSource);

   protected:
      lockable_critical_section *framebuffer_critical_section();

   private:
      lockable_critical_section *m_pcriticalsectionFramebuffer;

      ::pointer < ::remoting::CursorPositionDetector >  m_pcursorpositiondetector;
      ::pointer < WindowsCursorShapeGrabber > m_pcursorshapegrabber;
      ::pointer < ::remoting::CursorShapeDetector > m_pcursorshapedetector;

      ::pointer < ::remoting::CopyRectDetector > m_pcopyrectdetector;
   };

   //// __WIN32SCREENDRIVERBASEIMPL_H__


} // namespace remoting_windows
 


