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
#include "RfbServer.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "subsystem_bsd_sockets/socket/SocketAddressIPv4.h"


namespace remoting_rfb_node_desktop
{

   RfbServer::RfbServer(
      const ::scoped_string &scopedstrBindHost, 
      ::remoting_rfb_node::Configurator *pconfigurator,
                        unsigned short bindPort,
                        RfbClientManager *clientManager, bool lockAddr, ::subsystem::LogWriter * plogwriter,
                        const ::i32_rectangle &rectangleViewport) :
       TcpServer(scopedstrBindHost, bindPort, false, lockAddr), m_pconfigurator(pconfigurator),
      m_clientManager(clientManager), m_plogwriter(plogwriter)


   {
      if (rectangleViewport != 0)
      {
         m_rectangleViewport.setArbitraryRect(rectangleViewport);
      }

      TcpServer::start();

      if (rectangleViewport == 0)
      {
         m_plogwriter->debug("Rfb server started at {}:{}", scopedstrBindHost, (int)bindPort);
      }
      else
      {
         m_plogwriter->debug("Rfb server started at {}:{} with [{} {} {} {}] view port specified", scopedstrBindHost,
                             (int)bindPort, rectangleViewport.left, rectangleViewport.right, rectangleViewport.top, rectangleViewport.bottom);
      }
   }

   RfbServer::~RfbServer() { m_plogwriter->debug("Rfb server at {}:{} stopped", getBindHost(), (int)getBindPort()); }

   void RfbServer::onAcceptConnection(::subsystem::SocketIPv4Interface *socket)
   {
      try
      {
         // Get incoming connection address and convert it to string.
         ::pointer<::subsystem::SocketAddressIPv4Interface> peerAddr;
         peerAddr = socket->getPeerAddr();
         ::string peerIpString;
         peerIpString = peerAddr->toString();

         m_plogwriter->debug("Incoming rfb connection from {} to port %u", peerIpString, peerAddr->getPort());
         auto paddrImpl = peerAddr->impl<::subsystem_bsd_sockets::SocketAddressIPv4>();
         auto addr_in = paddrImpl->_getSockAddr();

         // Check access control rules for the IP address of the peer.
         // FIXME: Check loopback-related rules separately, report differently.
         // ::remoting_rfb_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();
         auto pserverconfig = m_pconfigurator->getServerConfig();
#ifdef WINDOWS
         ::remoting_rfb_node::IpAccessRule::ActionType action =
            pserverconfig->getActionByAddress((unsigned long)addr_in.sin_addr.S_un.S_addr);
#else
         ::remoting_rfb_node::IpAccessRule::ActionType action =
            pserverconfig->getActionByAddress((unsigned long)addr_in.sin_addr.s_addr);
#endif

         if (action == ::remoting_rfb_node::IpAccessRule::ACTION_TYPE_DENY)
         {
            m_plogwriter->debug("Connection rejected due to access control rules");
            delete socket;
            return;
         }

         // Access granted, add new RFB client. One more check will follow later in
         // RfbClientManager::onCheckAccessControl().

         socket->enableNaggleAlgorithm(false);

         m_clientManager->addNewConnection(socket, &m_rectangleViewport, false, false);
      }
      catch (::subsystem::Exception &ex)
      {
         m_plogwriter->error("Failed to process incoming rfb connection with following reason: \"{}\"",
                             ex.get_message());
      }
   }


} // namespace remoting_rfb_node_desktop
 


