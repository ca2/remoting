// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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
#include "ServerHelp.h"

//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

#include "subsystem/platform/StringTable.h"


#include "remoting/node_desktop/resource.h"


namespace remoting_node_desktop
{
   ServerHelp::ServerHelp()
   {

   }


   void ServerHelp::showUsage()
   {
      throw "show_usage_todo";
      //MessageBox(0,
      //MainSubsystem().StringTable().getString(IDS_TVNSERVER_USAGE),
      //MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER_USAGE),
      //::user::e_message_box_ok|::user::e_message_box_icon_exclamation);
   }
} // namespace remoting_node_desktop