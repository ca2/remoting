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


// FIXME: Remove this detectors when will be ready win8 detectors.
#include "remoting/remoting_windows/_common_header.h"

#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting/desktop/CursorPositionDetector.h"

#include "remoting/remoting/desktop/CursorShapeDetector.h"
#include "remoting/remoting_windows/desktop/WindowsCursorShapeGrabber.h"

#include "remoting/remoting/desktop/ScreenDriver.h"
#include "remoting/remoting_windows/desktop/WinVideoRegionUpdaterImpl.h"
#include "remoting/remoting/desktop/UpdateKeeper.h"
#include "remoting/remoting/desktop/UpdateListener.h"
//#include "log_writer/LogWriter.h"
#include "remoting/remoting_windows/desktop/Win8ScreenDriverImpl.h"
#include "remoting/remoting/desktop/CopyRectDetector.h"

namespace remoting_windows
{

   class CLASS_DECL_REMOTING_WINDOWS Win8ScreenDriver :
   virtual public WinVideoRegionUpdaterImpl
   {
   public:
      // (Note: This class  has no link to an external backup frame buffer and then it does not have
      // to use an external belonged mutex. Thread safe coordiantion commitments between this class
      // and external some elements should be entirely provided by an owner code.)

      //Win8ScreenDriver(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener, lockable_critical_section *pcriticalsectionFramebuffer,
        //               ::subsystem::LogWriter * plogwriter);
      Win8ScreenDriver();
      ~Win8ScreenDriver() override;


      void initialize_screen_driver(::remoting::Configurator * pconfigurator, ::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;

      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual ::int_size getScreenDimension();
      virtual bool grabFb(const ::int_rectangle & rectangle = {});
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool getScreenPropertiesChanged();
      virtual bool getScreenSizeChanged();
      virtual bool applyNewScreenProperties();

      virtual bool grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat);
      virtual const ::remoting::CursorShape *getCursorShape();
      virtual ::int_point getCursorPosition();

      virtual void getCopiedRegion(::int_rectangle &rectangleCopy, ::int_point & pointSource);

   private:
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
      lockable_critical_section *m_pcriticalsectionFramebuffer;
      ::pointer < ::remoting::UpdateKeeper > m_pupdatekeeper;
      ::pointer <::remoting::UpdateListener >m_pupdatelistener;
      // This member must be always gueranted non zero. Otherwise an excption must
      // be provided from the constructor of this class.
      ::pointer < Win8ScreenDriverImpl > m_pwin8screendriveriimpl;
      ::remoting::CopyRectDetector m_pcopyrectdetector;
      lockable_critical_section m_drvImplMutex;

      ::remoting::CursorShape m_cursorshape;

      bool m_detectionEnabled;
   };

   //// __WIN8SCREENDRIVER_H__

} // namespace remoting_windows





