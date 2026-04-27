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
#include "remoting/remoting_windows/desktop/Win32ScreenDriverBaseImpl.h"
#include "remoting/remoting/desktop/Poller.h"
#include "remoting/remoting/desktop/ConsolePoller.h"
#include "remoting/remoting/desktop/HooksUpdateDetector.h"
#include "remoting/remoting_windows/desktop/WindowsScreenGrabber.h"

namespace remoting
{

   class CLASS_DECL_REMOTING_WINDOWS Win32ScreenDriver : public Win32ScreenDriverBaseImpl
   {
   public:
      //Win32ScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ::innate_subsystem::Framebuffer *pframebuffer,
        //                lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);
      Win32ScreenDriver();
      ~Win32ScreenDriver() override;


      void initialize_screen_driver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;

      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      virtual ::int_size getScreenDimension();
      virtual bool grabFb(const ::int_rectangle & rectangle = 0);
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool getScreenPropertiesChanged();
      virtual bool getScreenSizeChanged();
      virtual bool applyNewScreenProperties();

   private:
      // This class  provides thread safed coordinations between the backup frame buffer and
      // the following objects.
      WindowsScreenGrabber m_pscreengrabber;
      Poller m_poller;
      ConsolePoller m_consolePoller;
      HooksUpdateDetector m_hooks;
   };


} // namespace remoting





