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
#include "remoting/remoting_windows/desktop/Win8ScreenDriver.h"
#include "subsystem/platform/Exception.h"

namespace remoting
{


   Win8ScreenDriver::Win8ScreenDriver(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                                      critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter) :
       WinVideoRegionUpdaterImpl(plogwriter), m_plogwriter(plogwriter), m_fbcritical_section(pcriticalsectionFramebuffer),
       m_pupdatekeeper(pupdatekeeper), m_pupdatelistener = pupdatelistener;, m_detectionEnabled(false)
   {
      m_plogwriter->debug("Win8ScreenDriver creating new Win8ScreenDriverImpl");
      critical_section_lock al(&m_drvImplMutex);
      m_drvImpl = new Win8ScreenDriverImpl(m_plogwriter, m_pupdatekeeper, m_fbcritical_section, m_pupdatelistener);
   }

   Win8ScreenDriver::~Win8ScreenDriver()
   {
      terminateDetection();
      critical_section_lock al(&m_drvImplMutex);
      if (m_drvImpl != 0)
      {
         delete m_drvImpl;
         m_drvImpl = 0;
      }
   }

   void Win8ScreenDriver::executeDetection()
   {
      critical_section_lock al(&m_drvImplMutex);
      m_detectionEnabled = true;
      m_drvImpl->executeDetection();
   }

   void Win8ScreenDriver::terminateDetection()
   {
      critical_section_lock al(&m_drvImplMutex);
      m_detectionEnabled = false;
      if (m_drvImpl != 0)
      {
         m_drvImpl->terminateDetection();
      }
   }

   ::int_size Win8ScreenDriver::getScreenDimension()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_drvImpl->getScreenBuffer()->getDimension();
   }

   ::innate_subsystem::Framebuffer *Win8ScreenDriver::getScreenBuffer()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_drvImpl->getScreenBuffer();
   }

   bool Win8ScreenDriver::grabFb(const ::int_rectangle & rectangle)
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_drvImpl->grabFb(rectangle);
   }

   bool Win8ScreenDriver::getScreenPropertiesChanged()
   {
      critical_section_lock al(&m_drvImplMutex);
      return !m_drvImpl->isValid();
   }

   bool Win8ScreenDriver::getScreenSizeChanged()
   {
      critical_section_lock al(&m_drvImplMutex);
      return !m_drvImpl->isValid();
   }

   bool Win8ScreenDriver::applyNewScreenProperties()
   {
      try
      {
         m_plogwriter->debug("Applying new screen properties, deleting old Win8ScreenDriverImpl");
         critical_section_lock al(&m_drvImplMutex);
         if (m_drvImpl != 0)
         {
            delete m_drvImpl;
            m_drvImpl = 0;
         }
         m_plogwriter->debug("Applying new screen properties, creating new Win8ScreenDriverImpl");
         Win8ScreenDriverImpl *drvImpl = new Win8ScreenDriverImpl(m_plogwriter, m_pupdatekeeper, m_fbcritical_section,
                                                                  m_pupdatelistener, m_detectionEnabled);
         m_drvImpl = drvImpl;
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
      m_drvImpl->updateCursorShape(&m_cursorShape);
      return !m_drvImpl->isValid();
   }

   const CursorShape *Win8ScreenDriver::getCursorShape() { return &m_cursorShape; }

   ::int_point Win8ScreenDriver::getCursorPosition()
   {
      critical_section_lock al(&m_drvImplMutex);
      return m_drvImpl->getCursorPosition();
   }

   void Win8ScreenDriver::getCopiedRegion(::int_rectangle &rectangleCopy, ::int_point & pointSource)
   {
      critical_section_lock al(m_fbcritical_section);
      m_copyRectDetector.detectWindowMovements(rectangleCopy, source);
   }


} // namespace remoting
