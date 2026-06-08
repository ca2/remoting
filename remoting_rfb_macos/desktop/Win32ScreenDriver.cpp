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
#include "remoting/remoting_rfb_windows/desktop/Win32ScreenDriver.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_rfb_windows
{


   // Win32ScreenDriver::Win32ScreenDriver(::remoting_rfb::UpdateKeeper * pupdatekeeper, ::remoting_rfb::UpdateListener * pupdatelistener,
   //                                      ::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer,
   //                                      ::subsystem::LogWriter * plogwriter) :
   //     Win32ScreenDriverBaseImpl(pupdatekeeper, pupdatelistener, pcriticalsectionFramebuffer, log),
   //     m_poller(pupdatekeeper, pupdatelistener, &m_pscreengrabber, pframebuffer, pcriticalsectionFramebuffer, log),
   //     m_consolePoller(pupdatekeeper, pupdatelistener, &m_pscreengrabber, pframebuffer, pcriticalsectionFramebuffer, log),
   //     m_hooks(pupdatekeeper, pupdatelistener, log)
   // {
   //    // At this point the screen driver has valid screen properties (provides by screen grabber).
   // }

   Win32ScreenDriver::Win32ScreenDriver()
   {


   }

   Win32ScreenDriver::~Win32ScreenDriver() { terminateDetection(); }

   void Win32ScreenDriver::initialize_screen_driver(::remoting_rfb_node::Configurator * pconfigurator, ::remoting_rfb::UpdateKeeper * pupdatekeeper, ::remoting_rfb::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer, lockable_critical_section *pcriticalsectionFramebuffer,
                                        ::subsystem::LogWriter * plogwriter) //:

       //m_poller
       //m_consolePoller(pupdatekeeper, pupdatelistener, &m_pscreengrabber, pframebuffer, pcriticalsectionFramebuffer, log),
       //m_hooks(pupdatekeeper, pupdatelistener, log)
   {
      m_pconfigurator = pconfigurator;
      m_screengrabber.initialize_screen_grabber(m_pconfigurator);
      Win32ScreenDriverBaseImpl::initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
      m_poller.initialize_poller(pconfigurator, pupdatekeeper, pupdatelistener, &m_screengrabber, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
      m_consolePoller.initialize_console_poller(pupdatekeeper, pupdatelistener, &m_screengrabber, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
      m_hooks.initialize_hooks_update_detector(pupdatekeeper, pupdatelistener, plogwriter);
      // At this point the screen driver has valid screen properties (provides by screen grabber).
   }


   void Win32ScreenDriver::executeDetection()
   {
      Win32ScreenDriverBaseImpl::executeDetection();
      m_poller.resumeThread();
      m_consolePoller.resumeThread();
      m_hooks.resumeThread();
   }

   void Win32ScreenDriver::terminateDetection()
   {
      m_poller.setThreadToFinish();
      m_consolePoller.setThreadToFinish();
      m_hooks.setThreadToFinish();

      Win32ScreenDriverBaseImpl::terminateDetection();

      m_poller.waitThreadToFinish();
      m_consolePoller.waitThreadToFinish();
      m_hooks.waitThreadToFinish();
   }

   ::i32_size Win32ScreenDriver::getScreenDimension()
   {
      critical_section_lock al(framebuffer_critical_section());
      return m_screengrabber.getScreenRect().size();
   }

   bool Win32ScreenDriver::grabFb(const ::i32_rectangle & rectangle)
   {
      critical_section_lock al(framebuffer_critical_section());
      return m_screengrabber.grab(rectangle);
   }

   ::innate_subsystem::Framebuffer *Win32ScreenDriver::getScreenBuffer() { return m_screengrabber.getScreenBuffer(); }

   bool Win32ScreenDriver::getScreenPropertiesChanged()
   {
      critical_section_lock al(framebuffer_critical_section());
      return m_screengrabber.getPropertiesChanged();
   }

   bool Win32ScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(framebuffer_critical_section());
      return m_screengrabber.getScreenSizeChanged();
   }

   bool Win32ScreenDriver::applyNewScreenProperties()
   {
      critical_section_lock al(framebuffer_critical_section());
      return m_screengrabber.applyNewProperties();
   }


} // namespace remoting_rfb_windows


