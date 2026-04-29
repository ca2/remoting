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
#include "remoting/remoting_windows/desktop/DesktopFactory.h"
#include "remoting/remoting_windows/desktop/DesktopClientImpl.h"
#include "remoting/remoting_windows/desktop/DesktopWinImpl.h"
#include "remoting/server_config/Configurator.h"


namespace remoting_windows
{

   DesktopFactory::DesktopFactory() {}

   DesktopFactory::~DesktopFactory() {}

   ::pointer < ::remoting::Desktop > DesktopFactory::createDesktop(
      ::remoting::Configurator * pconfigurator,
      ::subsystem::ClipboardListener *pclipboardlistenerExternal,
      ::remoting::UpdateSendingListener *pupdatesendinglistenerExternal,
      ::remoting::AbnormDeskTermListener *pdesktermlistenerExternal,
      ::subsystem::LogWriter * plogwriter)
   {

      if (pconfigurator->m_isConfiguringService)
      {

         auto pdesktopNew = allocateø DesktopClientImpl();

         pdesktopNew->initialize_desktop(
            pconfigurator,
            pclipboardlistenerExternal,
            pupdatesendinglistenerExternal,
            pdesktermlistenerExternal,
            plogwriter);

         return pdesktopNew;

      }
      else
      {

         auto pdesktopImpl = allocateø DesktopWinImpl();

         pdesktopImpl->initialize_desktop(
            pconfigurator,
            pclipboardlistenerExternal,
            pupdatesendinglistenerExternal,
            pdesktermlistenerExternal,
            plogwriter);


         return pdesktopImpl;

      }

      return nullptr;

   }


} // namespace remoting_windows
 



