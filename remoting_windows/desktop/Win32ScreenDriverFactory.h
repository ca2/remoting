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

#pragma once

#include "remoting/remoting_macos/_common_header.h"
#include "remoting/remoting/desktop/ScreenDriverFactory.h"
#include "remoting/remoting/node_config/ServerConfig.h"


namespace remoting_macos
{

   class CLASS_DECL_REMOTING_MACOS Win32ScreenDriverFactory :
      virtual public ::remoting::ScreenDriverFactory
   {
   public:
      // The factory stores pointer to a ::remoting_node::ServerConfig object and will use it on
      // createScreenDriver() function calls.
      //Win32ScreenDriverFactory(::remoting_node::ServerConfig *pserverconfig);
      Win32ScreenDriverFactory();
      ~Win32ScreenDriverFactory() override;

      void initialize_screen_driver_factory(::remoting_node::ServerConfig *pserverconfig) override;

      ::pointer < ::remoting::ScreenDriver > createScreenDriver(
         ::remoting_node::Configurator * pconfigurator,
         ::remoting::UpdateKeeper * pupdatekeeper,
         ::remoting::UpdateListener * pupdatelistener,
         ::innate_subsystem::Framebuffer *pframebuffer,
         lockable_critical_section *pcriticalsectionFramebuffer,
         ::subsystem::LogWriter * plogwriter) override;

   private:
      virtual ::pointer < ::remoting::ScreenDriver > createStandardScreenDriver(
      ::remoting_node::Configurator * pconfigurator,
      ::remoting::UpdateKeeper * pupdatekeeper,
      ::remoting::UpdateListener * pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter
      );
      virtual ::pointer < ::remoting::ScreenDriver > createMirrorScreenDriver(
      ::remoting_node::Configurator * pconfigurator,
      ::remoting::UpdateKeeper * pupdatekeeper,
      ::remoting::UpdateListener * pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter
      );

      bool isMirrorDriverAllowed();
      bool isD3DAllowed();

      ::pointer < ::remoting_node::ServerConfig > m_pserverconfig;
   };

   //// __WIN32SCREENDRIVERFACTORY_H__

} // namespace remoting_macos



