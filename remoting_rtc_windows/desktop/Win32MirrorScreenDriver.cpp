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
#include "framework.h"
#include "remoting/remoting_windows/desktop/Win32MirrorScreenDriver.h"
#include "subsystem/platform/Exception.h"


namespace remoting_windows
{


   // Win32MirrorScreenDriver::Win32MirrorScreenDriver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
   //                                                  lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) :
   //     Win32ScreenDriverBaseImpl(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log),
   //     m_mirrorDriver(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log)
   // {
   //    // At this point the screen driver has valid screen properties.
   // }

   Win32MirrorScreenDriver::Win32MirrorScreenDriver()
   {
      // At this point the screen driver has valid screen properties.
   }

   Win32MirrorScreenDriver::~Win32MirrorScreenDriver() { terminateDetection(); }


   void Win32MirrorScreenDriver::initialize_screen_driver(::remoting_rtc_node::Configurator * pconfigurator,::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                                                 lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)
   {
      Win32ScreenDriverBaseImpl::initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);

      construct_newø(m_mirrorDriver);
      m_mirrorDriver->initialize_mirror_screen_driver(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, plogwriter);
      // At this point the screen driver has valid screen properties.
   }

   void Win32MirrorScreenDriver::executeDetection()
   {
      Win32ScreenDriverBaseImpl::executeDetection();
      m_mirrorDriver->executeDetection();
   }

   void Win32MirrorScreenDriver::terminateDetection()
   {
      Win32ScreenDriverBaseImpl::terminateDetection();
      m_mirrorDriver->terminateDetection();
   }

   ::i32_size Win32MirrorScreenDriver::getScreenDimension() { return m_mirrorDriver->getScreenDimension(); }

   ::innate_subsystem::Framebuffer *Win32MirrorScreenDriver::getScreenBuffer()
   {
      return m_mirrorDriver->getScreenBuffer();
   }

   bool Win32MirrorScreenDriver::grabFb(const ::i32_rectangle & rectangle) { return m_mirrorDriver->grab(rectangle); }

   bool Win32MirrorScreenDriver::getScreenPropertiesChanged() { return m_mirrorDriver->getPropertiesChanged(); }

   bool Win32MirrorScreenDriver::getScreenSizeChanged() { return m_mirrorDriver->getScreenSizeChanged(); }

   bool Win32MirrorScreenDriver::applyNewScreenProperties() { return m_mirrorDriver->applyNewProperties(); }


} // namespace remoting_windows
