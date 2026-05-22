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


#include "remoting/client/ViewerInstance.h"
#include "remoting/remoting_rtc/network/TcpServer.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting_client
{
   class ConnectionListener :
    virtual public ::remoting_rtc::TcpServer
   {
   public:
      static ::string_literal DEFAULT_HOST;
      static const unsigned short DEFAULT_PORT = 5500;

      // HWND mainWindow is handle of main window of program.
      // this window received and processing messages "WM_USER_NEW_LISTENING"
      ConnectionListener(::subsystem::OperatingSystemApplicationInterface *application, unsigned short port = DEFAULT_PORT);

      virtual ~ConnectionListener();

      // this method return pointer to new listening connection, if him is exist, and 0 if isn't
      ::pointer< ::subsystem::SocketIPv4Interface > getNewConnection();

      unsigned short getBindPort() const;
   protected:
      void onAcceptConnection(::subsystem::SocketIPv4Interface *socket);

      ::subsystem::OperatingSystemApplicationInterface*m_poperatingsystemapplication;
      ::list_base<::pointer < ::subsystem::SocketIPv4Interface > > m_connections;
      lockable_critical_section m_criticalsectionConnections;
   };
}//namespace remoting_client
