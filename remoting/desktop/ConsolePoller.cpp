// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "ConsolePoller.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "acme/_operating_system.h"


namespace operating_system
{

CLASS_DECL_ACME ::i32_rectangle get_console_rect();

} // namespace operating_system


namespace remoting
{

   // ConsolePoller::ConsolePoller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
   //                              ScreenGrabber *pscreengrabber, ::innate_subsystem::Framebuffer *backupFramebuffer,
   //                              lockable_critical_section *framebufferMutex, ::subsystem::LogWriter * plogwriter) :
   //     UpdateDetector(pupdatekeeper, pupdatelistener), m_pscreengrabber(pscreengrabber),
   //     m_pframebufferBackup(backupFramebuffer), m_framebufferMutex(framebufferMutex), m_plogwriter(plogwriter)
   // {
   //    m_rectanglePolling.set(0, 0, 16, 16);
   // }

   ConsolePoller::ConsolePoller() :
   m_plogwriter(nullptr),m_pcriticalsectionFramebuffer(nullptr)
   {

   }


   ConsolePoller::~ConsolePoller()
   {
      terminate();
      wait();
   }

   void ConsolePoller::initialize_console_poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener,
                             ScreenGrabber *pscreengrabber, ::innate_subsystem::Framebuffer *pframebufferBackup,
                             lockable_critical_section *pcriticalsectionFramebuffer, ::subsystem::LogWriter * plogwriter)
   {
      initialize_update_detector(pupdatekeeper, pupdatelistener);
      m_pscreengrabber = pscreengrabber;
      m_pframebufferBackup = pframebufferBackup;
      m_pcriticalsectionFramebuffer = pcriticalsectionFramebuffer;
      m_plogwriter = plogwriter;
      m_rectanglePolling.set(0, 0, 16, 16);
   }

   void ConsolePoller::onTerminate() { m_intervalWaiter.set_happening(); }

   void ConsolePoller::execute()
   {
      m_plogwriter->information("console poller thread id = {}", (::iptr) getThreadId());

      ::i32_rectangle scanRect;
      Region region;
      while (!isTerminating())
      {
         ::i32_rectangle conRect = getConsoleRect();
         if (!conRect.is_empty())
         {
            int pollHeight = m_rectanglePolling.height();
            int pollWidth = m_rectanglePolling.width();

            {
               critical_section_lock al(m_pcriticalsectionFramebuffer);
               ::i32_rectangle offsetFb = m_pscreengrabber->getScreenRect();
               conRect.offset(-offsetFb.left, -offsetFb.top);
               ::innate_subsystem::Framebuffer *pframebufferScreen = m_pscreengrabber->getScreenBuffer();
               if (pframebufferScreen->isEqualTo(m_pframebufferBackup))
               {
                  m_pscreengrabber->grab(conRect);
                  for (int iRow = conRect.top; iRow < conRect.bottom; iRow += pollHeight)
                  {
                     for (int iCol = conRect.left; iCol < conRect.right; iCol += pollWidth)
                     {
                        scanRect.set(iCol, iRow, minimum(iCol + pollWidth, conRect.right),
                                         minimum(iRow + pollHeight, conRect.bottom));
                        if (!pframebufferScreen->cmpFrom(scanRect, m_pframebufferBackup, scanRect.left, scanRect.top))
                        {
                           region.addRect(scanRect);
                        }
                     }
                  }
               }
            }

            // Send event
            if (!region.is_empty())
            {
               m_pupdatekeeper->addChangedRegion(region);
               doUpdate();
            }
         }
         ::u32 pollInterval = 200;
         m_intervalWaiter.wait(pollInterval * 1_ms);
      }
   }

   ::i32_rectangle ConsolePoller::getConsoleRect()
   {
      ::i32_rectangle rectangle;
      
      rectangle = ::operating_system::get_console_rect();
      
      return rectangle;
   }


} // namespace remoting




