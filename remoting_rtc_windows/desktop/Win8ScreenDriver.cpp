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
#include "remoting/remoting_windows/desktop/Win8ScreenDriver.h"
#include "subsystem/platform/Exception.h"
#include "remoting/remoting_rtc/node_config/Configurator.h"

namespace remoting_windows
{


   // Win8ScreenDriver::Win8ScreenDriver(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
   //                                    lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) :
   //     //WinVideoRegionUpdaterImpl(plogwriter),
   //     //m_plogwriter(plogwriter), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer),
   //     m_pupdatekeeper(pupdatekeeper), m_pupdatelistener(pupdatelistener), m_detectionEnabled(false)
   // {
   //        WinVideoRegionUpdaterImpl::initialize_screen_driver(plogwriter);
   //    m_plogwriter->debug("Win8ScreenDriver creating new Win8ScreenDriverImpl");
   //    critical_section_lock al(&m_drvImplMutex);
   //    m_pwin8screendriveriimpl = new Win8ScreenDriverImpl(m_plogwriter, m_pupdatekeeper, m_pcriticalsectionFramebuffer, m_pupdatelistener);
   // }

   Win8ScreenDriver::Win8ScreenDriver() :
   m_detectionEnabled(false)
   {


   }

   Win8ScreenDriver::~Win8ScreenDriver()
   {
      terminateDetection();
//      critical_section_lock al(&m_drvImplMutex);
      // if (m_pwin8screendriveriimpl != 0)
      // {
      //    delete m_pwin8screendriveriimpl;
      //    m_pwin8screendriveriimpl = 0;
      // }
   }


   void Win8ScreenDriver::initialize_screen_driver(::remoting_rtc_node::Configurator * pconfigurator,
      ::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener,
                                        ::innate_subsystem::Framebuffer *pframebuffer,
                          lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)// :
       // //WinVideoRegionUpdaterImpl(plogwriter),
       //m_plogwriter(plogwriter), m_pcriticalsectionFramebuffer(pcriticalsectionFramebuffer)
       // m_pupdatekeeper(pupdatekeeper), m_pupdatelistener(pupdatelistener), m_detectionEnabled(false)
   {
      m_plogwriter = plogwriter;
      m_pcriticalsectionFramebuffer = pcriticalsectionFramebuffer;
      m_pupdatekeeper = pupdatekeeper;
      m_pupdatelistener =  pupdatelistener;
      m_pconfigurator = pconfigurator;
      WinVideoRegionUpdaterImpl::initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
      m_plogwriter->debug("Win8ScreenDriver creating new Win8ScreenDriverImpl");
      critical_section_lock al(&m_drvImplMutex);
      emplace_newø(m_pwin8screendriveriimpl);
      m_pwin8screendriveriimpl->initialize_win8_screen_driver_impl(m_plogwriter, m_pupdatekeeper, m_pcriticalsectionFramebuffer, m_pupdatelistener);
   }

   void Win8ScreenDriver::executeDetection()
   {
      critical_section_lock al(&m_drvImplMutex);
      m_detectionEnabled = true;
      m_pwin8screendriveriimpl->executeDetection();
   }

   void Win8ScreenDriver::terminateDetection()
   {
      critical_section_lock al(&m_drvImplMutex);
      m_detectionEnabled = false;
      if (m_pwin8screendriveriimpl != 0)
      {
         m_pwin8screendriveriimpl->terminateDetection();
      }
   }

   ::i32_size Win8ScreenDriver::getScreenDimension()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_pwin8screendriveriimpl->getScreenBuffer()->getDimension();
   }

   ::innate_subsystem::Framebuffer *Win8ScreenDriver::getScreenBuffer()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_pwin8screendriveriimpl->getScreenBuffer();
   }

   bool Win8ScreenDriver::grabFb(const ::i32_rectangle & rectangle)
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_pwin8screendriveriimpl->grabFb(rectangle);
   }

   bool Win8ScreenDriver::getScreenPropertiesChanged()
   {
      critical_section_lock al(&m_drvImplMutex);
      return !m_pwin8screendriveriimpl->isValid();
   }

   bool Win8ScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(&m_drvImplMutex);
      return !m_pwin8screendriveriimpl->isValid();
   }

   bool Win8ScreenDriver::applyNewScreenProperties()
   {
      try
      {
         m_plogwriter->debug("Applying new screen properties, deleting old Win8ScreenDriverImpl");
         critical_section_lock al(&m_drvImplMutex);
         if (m_pwin8screendriveriimpl != 0)
         {
            delete m_pwin8screendriveriimpl;
            m_pwin8screendriveriimpl = 0;
         }
         m_plogwriter->debug("Applying new screen properties, creating new Win8ScreenDriverImpl");
         Win8ScreenDriverImpl *drvImpl = new Win8ScreenDriverImpl();
         drvImpl->initialize_win8_screen_driver_impl(m_plogwriter, m_pupdatekeeper, m_pcriticalsectionFramebuffer,
                                                                  m_pupdatelistener, m_detectionEnabled);
         m_pwin8screendriveriimpl = drvImpl;
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Can't apply new screen properties: {}", e.get_message());
         return false;
      }
      return true;
   }

   bool Win8ScreenDriver::grabCursorShape(const ::innate_subsystem::PixelFormat & pixelformat)
   {
      critical_section_lock al(&m_drvImplMutex);
      m_pwin8screendriveriimpl->updateCursorShape(&m_cursorshape);
      return !m_pwin8screendriveriimpl->isValid();
   }

   const ::remoting_rtc::CursorShape *Win8ScreenDriver::getCursorShape() { return &m_cursorshape; }

   ::i32_point Win8ScreenDriver::getCursorPosition()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_pwin8screendriveriimpl->getCursorPosition();
   }

   void Win8ScreenDriver::getCopiedRegion(::i32_rectangle &rectangleCopy, ::i32_point & pointSource)
   {
      critical_section_lock al(m_pcriticalsectionFramebuffer);
      m_pcopyrectdetector.detectWindowMovements(rectangleCopy, pointSource);
   }


} // namespace remoting_windows
