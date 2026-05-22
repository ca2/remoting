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

#pragma once

#include "remoting/remoting_rtc_windows/_common_header.h"
#include "remoting/remoting_rtc/desktop/ScreenDriverFactory.h"
#include "remoting/remoting_rtc/node_config/ServerConfig.h"


namespace remoting_rtc_windows
{

   class CLASS_DECL_REMOTING_RTC_WINDOWS Win32ScreenDriverFactory :
      virtual public ::remoting_rtc::ScreenDriverFactory
   {
   public:
      // The factory stores pointer to a ::remoting_rtc_node::ServerConfig object and will use it on
      // createScreenDriver() function calls.
      //Win32ScreenDriverFactory(::remoting_rtc_node::ServerConfig *pserverconfig);
      Win32ScreenDriverFactory();
      ~Win32ScreenDriverFactory() override;

      void initialize_screen_driver_factory(::remoting_rtc_node::ServerConfig *pserverconfig) override;

      ::pointer < ::remoting_rtc::ScreenDriver > createScreenDriver(
         ::remoting_rtc_node::Configurator * pconfigurator,
         ::remoting_rtc::UpdateKeeper * pupdatekeeper,
         ::remoting_rtc::UpdateListener * pupdatelistener,
         ::innate_subsystem::Framebuffer *pframebuffer,
         lockable_critical_section *pcriticalsectionFramebuffer,
         ::subsystem::LogWriter * plogwriter) override;

   private:
      virtual ::pointer < ::remoting_rtc::ScreenDriver > createStandardScreenDriver(
      ::remoting_rtc_node::Configurator * pconfigurator,
      ::remoting_rtc::UpdateKeeper * pupdatekeeper,
      ::remoting_rtc::UpdateListener * pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter
      );
      virtual ::pointer < ::remoting_rtc::ScreenDriver > createMirrorScreenDriver(
      ::remoting_rtc_node::Configurator * pconfigurator,
      ::remoting_rtc::UpdateKeeper * pupdatekeeper,
      ::remoting_rtc::UpdateListener * pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter
      );

      bool isMirrorDriverAllowed();
      bool isD3DAllowed();

      ::pointer < ::remoting_rtc_node::ServerConfig > m_pserverconfig;
   };

   //// __WIN32SCREENDRIVERFACTORY_H__

} // namespace remoting_rtc_windows



