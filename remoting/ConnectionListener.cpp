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
#include "framework.h"
#include "ConnectionListener.h"
#include "remoting_impact.h"

//#include "subsystem/thread/critical_section.h"

namespace remoting_remoting
{
   const char ConnectionListener::DEFAULT_HOST[] = "0.0.0.0";

   ConnectionListener::ConnectionListener(::subsystem::OperatingSystemApplicationInterface   *application,
                                          unsigned short port)
   : TcpServer(DEFAULT_HOST, port, true),
     m_application(application)
   {
   }

   ConnectionListener::~ConnectionListener()
   {
      critical_section_lock al(&m_connectionsLock);
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
      critical_section_lock al(&m_connectionsLock);
      m_connections.push_front(socket);
      m_application->postMainThreadMessage(remoting_impact::_WM_USER_NEW_LISTENING);
   }

   ::pointer < ::subsystem::SocketIPv4Interface > ConnectionListener::getNewConnection()
   {
      critical_section_lock al(&m_connectionsLock);
      ::subsystem::SocketIPv4Interface *socket = nullptr;
      if (!m_connections.empty()) {
         socket = m_connections.front();
         m_connections.pop_front();
      }
      return socket;
   }
}// namespace remoting_remoting
