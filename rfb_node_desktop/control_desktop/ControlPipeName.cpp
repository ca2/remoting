// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "ControlPipeName.h"

//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

//#include "remoting/rfb_node_desktop/NamingDefs.h"
//#include "subsystem_windows/platform/subsystem.h"
//#include "subsystem_windows/node/WTS.h"
#include "subsystem/node/OperatingSystem.h"

namespace remoting_control_desktop
{
   void ControlPipeName::createPipeName(bool forService, ::string & pipeName, ::subsystem::LogWriter * plogwriter)
   {
      if (forService) {
         pipeName =
           //ServerApplicationNames::FOR_SERVICE_CONTROL_APP_PIPE_NAME
           "Remoting_Service_Control"
           ;
      } else {

         int iActiveConsoleSessionId = MainSubsystem().OperatingSystem().getActiveConsoleSessionId(plogwriter);
         pipeName.format("{}_On_Session_{}",
           //ServerApplicationNames::FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME,
           "Remoting_Application_Control",
           iActiveConsoleSessionId);
      }
   }
} // namespace remoting_control_desktop

