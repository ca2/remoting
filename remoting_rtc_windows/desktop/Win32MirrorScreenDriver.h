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

#include "remoting/remoting_windows/_common_header.h"
#include "remoting/remoting_windows/desktop/MirrorScreenDriver.h"
#include "remoting/remoting_windows/desktop/Win32ScreenDriverBaseImpl.h"

namespace remoting_windows
{

   class CLASS_DECL_REMOTING_WINDOWS Win32MirrorScreenDriver : public Win32ScreenDriverBaseImpl
   {
   public:
      //Win32MirrorScreenDriver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
        //                      lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter);


      Win32MirrorScreenDriver();
      ~Win32MirrorScreenDriver() override;


      void initialize_screen_driver(::remoting_rtc_node::Configurator * pconfigurator, ::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) override;


      // Starts screen update detection if it not started yet.
      virtual void executeDetection();

      // Stops screen update detection.
      virtual void terminateDetection();

      // Note: This class  is a wrapper for the MirrorScreenDriver class CLASS_DECL_REMOTING_WINDOWS and then
      // only MirrorScreenDriver can provide appropriate thread safety for the ::remoting_rtc::ScreenDriver functions.

      virtual ::i32_size getScreenDimension();
      virtual bool grabFb(const ::i32_rectangle & rectangle = {});
      virtual ::innate_subsystem::Framebuffer *getScreenBuffer();
      virtual bool getScreenPropertiesChanged();
      virtual bool getScreenSizeChanged();
      virtual bool applyNewScreenProperties();

   private:
      ::pointer < MirrorScreenDriver > m_mirrorDriver;
   };

   //// __WIN32MIRRORSCREENDRIVER_H__


} // namespace remoting_windows




