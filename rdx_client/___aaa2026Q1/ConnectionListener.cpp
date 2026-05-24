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
#include "../ConnectionListener.h"
#include "../framework.h"
#include "../remoting_impact.h"

// #include "subsystem/thread/lockable_critical_section.h"

namespace remoting_client
{
   ::string_literal ConnectionListener::DEFAULT_HOST = "0.0.0.0";

   ConnectionListener::ConnectionListener(::subsystem::OperatingSystemApplicationInterface   *application,
                                          unsigned short port)
   : TcpServer(DEFAULT_HOST, port, true),
     m_poperatingsystemapplication(application)
   {
   }

   ConnectionListener::~ConnectionListener()
   {
      critical_section_lock al(&m_criticalsectionConnections);
      while (!m_connections.empty()) {
         ::subsystem::SocketIPv4Interface *socket = m_connections.front();
         delete socket;
         m_connections.pop_front();
      }
   }

   unsigned short ConnectionListener::getBindPort() const
   {
      return TcpServer::getBindPort();
   }

   void ConnectionListener::onAcceptConnection(::subsystem::SocketIPv4Interface *socket)
   {
      critical_section_lock al(&m_criticalsectionConnections);
      m_connections.push_front(socket);
      m_poperatingsystemapplication->postMessage(remoting_impact::_WM_USER_NEW_LISTENING);
   }

   ::pointer < ::subsystem::SocketIPv4Interface > ConnectionListener::getNewConnection()
   {
      critical_section_lock al(&m_criticalsectionConnections);
      ::subsystem::SocketIPv4Interface *socket = nullptr;
      if (!m_connections.empty()) {
         socket = m_connections.front();
         m_connections.pop_front();
      }
      return socket;
   }
}// namespace remoting_client
