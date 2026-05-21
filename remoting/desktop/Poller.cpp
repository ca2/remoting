// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "subsystem/_common_header.h"
#include "Poller.h"
#include "remoting/remoting/region/Region.h"
#include "remoting/remoting/node_config/Configurator.h"

namespace remoting
{

   //
   // Poller::Poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
   //                ::innate_subsystem::Framebuffer *backupFramebuffer, lockable_critical_section *framebufferCriticalSection,
   //                ::subsystem::LogWriter * plogwriter) :
   //     UpdateDetector(pupdatekeeper, pupdatelistener), m_pscreengrabber(pscreengrabber),
   //     m_pframebufferBackup(backupFramebuffer), m_pcriticalsectionFramebuffer(framebufferCriticalSection), m_plogwriter = plogwriter;
   // {
   //    m_pollingRect.set(0, 0, 16, 16);
   // }

   Poller::Poller()
   {


   }

   Poller::~Poller()
   {
     // setThreadToFinish();
      //waitThreadToFinish();
   }

void Poller::destroy()
{
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
}


void Poller::initialize_poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
               ::innate_subsystem::Framebuffer *backupFramebuffer, lockable_critical_section *framebufferCriticalSection,
               ::subsystem::LogWriter * plogwriter)
   {

      initialize_update_detector(pupdatekeeper, pupdatelistener);
      m_pscreengrabber = pscreengrabber;
      m_pframebufferBackup = backupFramebuffer;
      m_pcriticalsectionFramebuffer = framebufferCriticalSection;
      m_plogwriter = plogwriter;
      m_rectanglePolling.set(0, 0, 16, 16);

   }

   void Poller::onTermThread() { m_intervalWaiter.set_happening(); }

   void Poller::onThreadMain()
   {
      m_plogwriter->information("poller thread id = {}", (::iptr) getThreadId());

      ::innate_subsystem::Framebuffer *pframebufferScreen;

      {
         critical_section_lock al(m_pcriticalsectionFramebuffer);
         pframebufferScreen = m_pscreengrabber->getScreenBuffer();
         ::i32_rectangle fullScreenRect(pframebufferScreen->getDimension());
         m_pupdatekeeper->addChangedRect(fullScreenRect);
      }

      while (!isThreadTerminating())
      {
         Region region;

         {
            critical_section_lock al(m_pcriticalsectionFramebuffer);

            pframebufferScreen = m_pscreengrabber->getScreenBuffer();
            if (!pframebufferScreen->isEqualTo(m_pframebufferBackup))
            {
               m_pupdatekeeper->setScreenSizeChanged();
            }
            else
            {
               m_plogwriter->information("grabbing screen for polling");
               m_pscreengrabber->grab();
               m_plogwriter->information("end of grabbing screen for polling");

               // Polling
               int pollingWidth = m_rectanglePolling.width();
               int pollingHeight = m_rectanglePolling.height();
               int screenWidth = pframebufferScreen->getDimension().cx;
               int screenHeight = pframebufferScreen->getDimension().cy;

               ::i32_rectangle scanRect;
               for (int iRow = 0; iRow < screenHeight; iRow += pollingHeight)
               {
                  for (int iCol = 0; iCol < screenWidth; iCol += pollingWidth)
                  {
                     scanRect.set(iCol, iRow, minimum(iCol + pollingWidth, screenWidth),
                                      minimum(iRow + pollingHeight, screenHeight));
                     if (!pframebufferScreen->cmpFrom(scanRect, m_pframebufferBackup, scanRect.left, scanRect.top))
                     {
                        region.addRect(scanRect);
                     }
                  }
               }

               m_pupdatekeeper->addChangedRegion(region);
            }
         } // critical_section_lock

         // Send event
         if (!region.is_empty())
         {
            doUpdate();
         }

         ::u32 pollInterval = m_pconfigurator->getServerConfig()->getPollingInterval();
         m_intervalWaiter.wait(pollInterval * 1_ms);
      }
   }


} // namespace remoting


