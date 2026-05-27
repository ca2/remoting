// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "TcpServer.h"

#include "subsystem/socket/SocketAddressIPv4.h"


namespace remoting_rfb
{
   TcpServer::TcpServer(const ::scoped_string & scopedstrBindHost, ::u16 bindPort,
                        bool autoStart,
                        bool lockAddr)
   : m_bindHost(scopedstrBindHost), m_bindPort(bindPort)
   {
      auto paddressBind = MainSubsystem().resolve_ip4_address(scopedstrBindHost, bindPort);

#ifdef WINDOWS
      if (lockAddr) {
         m_listenSocket.setExclusiveAddrUse();
      }
#endif

      m_listenSocket.bind(paddressBind);
      m_listenSocket.listen(10);

      if (autoStart) {
         start();
      }
   }

   TcpServer::~TcpServer()
   {

   }

void TcpServer::destroy()
{
   
   ::subsystem::Thread::destroy();
   
   try { m_listenSocket.shutdown(::subsystem::e_socket_shutdown_both); } catch(...) { }
   try { m_listenSocket.close(); } catch (...) { }

//   if (isThreadActive()) {
//      Thread::terminateThread();
//      Thread::waitThreadToFinish();
//   }
   
}

   ::string TcpServer::getBindHost() const
   {
      return m_bindHost;
   }

   ::u16 TcpServer::getBindPort() const
   {
      return m_bindPort;
   }

   void TcpServer::start()
   {
      resumeThread();
   }

   void TcpServer::onThreadMain()
   {
      while (!isThreadTerminating()) {
         ::pointer < ::subsystem::SocketIPv4Interface > clientSocket;

         try {
            clientSocket = m_listenSocket.accept();
         } catch (...) {
            clientSocket = NULL;
         }

         if (clientSocket != NULL) {
            onAcceptConnection(clientSocket);
         } else {
            break ;
         }
      }
   }
} // namespace remoting_rfb
