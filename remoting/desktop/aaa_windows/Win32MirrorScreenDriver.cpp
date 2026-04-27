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
#include "framework.h"
#include "../Win32MirrorScreenDriver.h"
#include "subsystem/platform/Exception.h"


namespace remoting
{


   Win32MirrorScreenDriver::Win32MirrorScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                                    lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) :
       Win32ScreenDriverBaseImpl(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log),
       m_mirrorDriver(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log)
   {
      // At this point the screen driver has valid screen properties.
   }

   Win32MirrorScreenDriver::~Win32MirrorScreenDriver() { terminateDetection(); }

   void Win32MirrorScreenDriver::executeDetection()
   {
      Win32ScreenDriverBaseImpl::executeDetection();
      m_mirrorDriver.executeDetection();
   }

   void Win32MirrorScreenDriver::terminateDetection()
   {
      Win32ScreenDriverBaseImpl::terminateDetection();
      m_mirrorDriver.terminateDetection();
   }

   ::int_size Win32MirrorScreenDriver::getScreenDimension() { return m_mirrorDriver.getScreenDimension(); }

   ::innate_subsystem::Framebuffer *Win32MirrorScreenDriver::getScreenBuffer()
   {
      return m_mirrorDriver.getScreenBuffer();
   }

   bool Win32MirrorScreenDriver::grabFb(const ::int_rectangle & rectangle) { return m_mirrorDriver.grab(rectangle); }

   bool Win32MirrorScreenDriver::getScreenPropertiesChanged() { return m_mirrorDriver.getPropertiesChanged(); }

   bool Win32MirrorScreenDriver::getScreenSizeChanged() { return m_mirrorDriver.getScreenSizeChanged(); }

   bool Win32MirrorScreenDriver::applyNewScreenProperties() { return m_mirrorDriver.applyNewProperties(); }


} // namespace remoting
