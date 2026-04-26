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
#include "../Win32ScreenDriver.h"
//#include "subsystem/thread/critical_section.h"

namespace remoting
{


   Win32ScreenDriver::Win32ScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer, critical_section *pcriticalsectionFramebuffer,
                                        ::subsystem::LogWriter * plogwriter) :
       Win32ScreenDriverBaseImpl(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log),
       m_poller(pupdatekeeper, pupdatelistener, &m_pscreengrabber, pframebuffer, pcriticalsectionFramebuffer, log),
       m_consolePoller(pupdatekeeper, pupdatelistener, &m_pscreengrabber, pframebuffer, pcriticalsectionFramebuffer, log),
       m_hooks(pupdatekeeper, pupdatelistener, log)
   {
      // At this point the screen driver has valid screen properties (provides by screen grabber).
   }

   Win32ScreenDriver::~Win32ScreenDriver() { terminateDetection(); }

   void Win32ScreenDriver::executeDetection()
   {
      Win32ScreenDriverBaseImpl::executeDetection();
      m_poller.resume();
      m_consolePoller.resume();
      m_hooks.resume();
   }

   void Win32ScreenDriver::terminateDetection()
   {
      m_poller.terminate();
      m_consolePoller.terminate();
      m_hooks.terminate();

      Win32ScreenDriverBaseImpl::terminateDetection();

      m_poller.wait();
      m_consolePoller.wait();
      m_hooks.wait();
   }

   ::int_size Win32ScreenDriver::getScreenDimension()
   {
      critical_section_lock al(getFbMutex());
      return ::int_size(&m_pscreengrabber.getScreenRect());
   }

   bool Win32ScreenDriver::grabFb(const ::int_rectangle & rectangle)
   {
      critical_section_lock al(getFbMutex());
      return m_pscreengrabber.grab(rectangle);
   }

   ::innate_subsystem::Framebuffer *Win32ScreenDriver::getScreenBuffer() { return m_pscreengrabber.getScreenBuffer(); }

   bool Win32ScreenDriver::getScreenPropertiesChanged()
   {
      critical_section_lock al(getFbMutex());
      return m_pscreengrabber.getPropertiesChanged();
   }

   bool Win32ScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(getFbMutex());
      return m_pscreengrabber.getScreenSizeChanged();
   }

   bool Win32ScreenDriver::applyNewScreenProperties()
   {
      critical_section_lock al(getFbMutex());
      return m_pscreengrabber.applyNewProperties();
   }


} // namespace remoting


