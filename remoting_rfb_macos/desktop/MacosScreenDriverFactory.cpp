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
#include "remoting/remoting_rfb_macos/desktop/MacosScreenDriverFactory.h"
#include "remoting/remoting_rfb_macos/desktop/MacosScreenDriver.h"

namespace remoting_rfb_macos
{

   MacosScreenDriverFactory::MacosScreenDriverFactory()
   {
   }

   MacosScreenDriverFactory::~MacosScreenDriverFactory()
   {
   }

   ::pointer < ::remoting_rfb::ScreenDriver > MacosScreenDriverFactory::createScreenDriver(
      ::remoting_rfb_node::Configurator * pconfigurator,
      ::remoting_rfb::UpdateKeeper *pupdatekeeper,
      ::remoting_rfb::UpdateListener *pupdatelistener,
      ::innate_subsystem::Framebuffer *pframebuffer,
      lockable_critical_section *pcriticalsectionFramebuffer,
      ::subsystem::LogWriter * plogwriter)
   {
      plogwriter->information("Using the CoreGraphics macOS screen driver");
      auto pscreendriver = allocateø MacosScreenDriver();

      pscreendriver->initialize_screen_driver(pconfigurator, pupdatekeeper, pupdatelistener, pframebuffer, pcriticalsectionFramebuffer, plogwriter);

      return pscreendriver;
   }

} // namespace remoting_rfb_macos


