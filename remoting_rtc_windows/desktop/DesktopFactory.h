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
#include "remoting/remoting_rtc/desktop/DesktopFactory.h"

namespace remoting_rtc_windows
{


   class CLASS_DECL_REMOTING_RTC_WINDOWS DesktopFactory :
      virtual public ::remoting_rtc::DesktopFactory
   {
   public:


      DesktopFactory();
      ~DesktopFactory();

      ::pointer < ::remoting_rtc::Desktop > createDesktop(
         ::remoting_rtc_node::Configurator * pconfigurator,
         ::subsystem::ClipboardListener *pclipboardlistenerExternal,
         ::remoting_rtc::UpdateSendingListener *pupdatesendinglistenerExternal,
         ::remoting_rtc::AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) override;

   };


} // namespace remoting_rtc_windows






