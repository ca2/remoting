// Copyright (C) 2012 GlavSoft LLC.
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
#include "remoting/remoting_macos/desktop/DesktopFactory.h"

#include "acme/platform/ProfileLogger.h"
#include "remoting/remoting_macos/desktop/DesktopClientImpl.h"
#include "remoting/remoting_macos/desktop/DesktopWinImpl.h"
#include "remoting/remoting/node_config/Configurator.h"


namespace remoting_macos
{

   DesktopFactory::DesktopFactory()
   {

      ::deferProfileLogger();
   }

   DesktopFactory::~DesktopFactory() {}

   ::pointer < ::remoting::Desktop > DesktopFactory::createDesktop(
      ::remoting_node::Configurator * pconfigurator,
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


} // namespace remoting_macos
 



