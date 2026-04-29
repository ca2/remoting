// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "ConnectStringParser.h"

#include "subsystem/platform/StringParser.h"

namespace remoting_control_desktop
{
   ConnectStringParser::ConnectStringParser()
   {
   }

   ConnectStringParser::~ConnectStringParser()
   {
   }

   bool ConnectStringParser::parse(const ::scoped_string & scopedstrConnectString,
                                   ::string * pstrHost,
                                   unsigned short * pushPort)
   {
      ::string connStrStorage(scopedstrConnectString);

      ::string_array splitted;

      splitted.explode(":", scopedstrConnectString);

      if (splitted.size() != 2)
      {
         return false;
      }

      int port = 0;

      if (!MainSubsystem().StringParser().parseInt(splitted[1], &port)) {
         return false;
      }

      if (port <= 0 || port > 65535) {
         return false;
      }

      if (pstrHost)
      {
         *pstrHost = splitted[0];
      }

      if (pushPort)
      {
         *pushPort = (unsigned short)port;
      }

      return true;
   }
} // namespace remoting_control_desktop

