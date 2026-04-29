// Copyright (C) 2012 GlavSoft LLC.
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
#include "remoting/remoting_windows/desktop/Win32ScreenDriverFactory.h"
#include "remoting/remoting_windows/desktop/Win32MirrorScreenDriver.h"
#include "remoting/remoting_windows/desktop/Win32ScreenDriver.h"
#include "remoting/remoting_windows/desktop/Win8ScreenDriver.h"

namespace remoting_windows
{


   Win32ScreenDriverFactory::Win32ScreenDriverFactory()
   {

   }
      //::remoting::ServerConfig *pserverconfig) : m_pserverconfig(pserverconfig) {}

   Win32ScreenDriverFactory::~Win32ScreenDriverFactory() {}

   void Win32ScreenDriverFactory::initialize_screen_driver_factory(::remoting::ServerConfig *pserverconfig)
   {

      m_pserverconfig = pserverconfig;

   }


   ::pointer < ::remoting::ScreenDriver > Win32ScreenDriverFactory::createScreenDriver(
      ::remoting::Configurator * pconfigurator,
      ::remoting::UpdateKeeper *pupdatekeeper,
      ::remoting::UpdateListener *pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter)
   {
      // Try to use Win8 duplication API firstly because it's in preference to other methods.
      if (isD3DAllowed())
      {
         plogwriter->information("D3D driver usage is allowed, try to start it...");
         try
         {
            auto pscreendriver = allocateø Win8ScreenDriver();

            pscreendriver->initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);

            return pscreendriver;
         }
         catch (::exception &e)
         {
            plogwriter->error("The Win8 duplication api can't be used: {}", e.get_message());
         }
      }
      else
      {
         plogwriter->information("D3D driver usage is disallowed");
      }

      if (isMirrorDriverAllowed())
      {
         plogwriter->information("Mirror driver usage is allowed, try to start it...");
         try
         {
            return createMirrorScreenDriver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
         }
         catch (::exception &e)
         {
            plogwriter->error("The mirror driver factory has failed: {}", e.get_message());
         }
      }
      else
      {
         plogwriter->information("Mirror driver usage is disallowed");
      }
      plogwriter->information("Using the standart screen driver");
      return createStandardScreenDriver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);
   }

   ::pointer < ::remoting::ScreenDriver > Win32ScreenDriverFactory::createStandardScreenDriver(
      ::remoting::Configurator * pconfigurator,
      ::remoting::UpdateKeeper *pupdatekeeper,
      ::remoting::UpdateListener *pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter)
   {
      auto pscreendriver = allocateø Win32ScreenDriver();

      pscreendriver->initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);

      return pscreendriver;




   }

   ::pointer < ::remoting::ScreenDriver > Win32ScreenDriverFactory::createMirrorScreenDriver(
      ::remoting::Configurator * pconfigurator,
      ::remoting::UpdateKeeper *pupdatekeeper,
      ::remoting::UpdateListener *pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter)
   {
      auto pscreendriver = allocateø Win32MirrorScreenDriver();

      pscreendriver->initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer,  pcriticalsectionFramebuffer, plogwriter);

      return pscreendriver;

   }

   bool Win32ScreenDriverFactory::isMirrorDriverAllowed() { return m_pserverconfig->getMirrorIsAllowed(); }

   bool Win32ScreenDriverFactory::isD3DAllowed() { return m_pserverconfig->getD3DIsAllowed(); }
} // namespace remoting_windows



