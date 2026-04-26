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
#include "../Win32ScreenDriverFactory.h"
#include "Win32MirrorScreenDriver.h"
#include "Win32ScreenDriver.h"
#include "Win8ScreenDriver.h"

namespace remoting
{


   Win32ScreenDriverFactory::Win32ScreenDriverFactory(ServerConfig *srvConf) : m_srvConf(srvConf) {}

   Win32ScreenDriverFactory::~Win32ScreenDriverFactory() {}

   ScreenDriver *Win32ScreenDriverFactory::createScreenDriver(UpdateKeeper *pupdatekeeper,
                                                              UpdateListener *pupdatelistener,
                                                              ::innate_subsystem::Framebuffer *pframebuffer,
                                                              critical_section *fbcritical_section,
                                                              ::subsystem::LogWriter * plogwriter)
   {
      // Try to use Win8 duplication API firstly because it's in preference to other methods.
      if (isD3DAllowed())
      {
         log->information("D3D driver usage is allowed, try to start it...");
         try
         {
            return new Win8ScreenDriver(pupdatekeeper, pupdatelistener, fbcritical_section, log);
         }
         catch (::exception &e)
         {
            log->error("The Win8 duplication api can't be used: {}", e.get_message());
         }
      }
      else
      {
         log->information("D3D driver usage is disallowed");
      }

      if (isMirrorDriverAllowed())
      {
         log->information("Mirror driver usage is allowed, try to start it...");
         try
         {
            return createMirrorScreenDriver(pupdatekeeper, pupdatelistener, fbcritical_section, log);
         }
         catch (::exception &e)
         {
            log->error("The mirror driver factory has failed: {}", e.get_message());
         }
      }
      else
      {
         log->information("Mirror driver usage is disallowed");
      }
      log->information("Using the standart screen driver");
      return createStandardScreenDriver(pupdatekeeper, pupdatelistener, pframebuffer, fbcritical_section, log);
   }

   ScreenDriver *Win32ScreenDriverFactory::createStandardScreenDriver(UpdateKeeper *pupdatekeeper,
                                                                      UpdateListener *pupdatelistener,
                                                                      ::innate_subsystem::Framebuffer *pframebuffer,
                                                                      critical_section *fbcritical_section,
                                                                      ::subsystem::LogWriter * plogwriter)
   {
      return new Win32ScreenDriver(pupdatekeeper, pupdatelistener, pframebuffer, fbcritical_section, log);
   }

   ScreenDriver *Win32ScreenDriverFactory::createMirrorScreenDriver(UpdateKeeper *pupdatekeeper,
                                                                    UpdateListener *pupdatelistener,
                                                                    critical_section *fbcritical_section,
                                                                    ::subsystem::LogWriter * plogwriter)
   {
      return new Win32MirrorScreenDriver(pupdatekeeper, pupdatelistener, fbcritical_section, log);
   }

   bool Win32ScreenDriverFactory::isMirrorDriverAllowed() { return m_srvConf->getMirrorIsAllowed(); }

   bool Win32ScreenDriverFactory::isD3DAllowed() { return m_srvConf->getD3DIsAllowed(); }
} // namespace remoting



