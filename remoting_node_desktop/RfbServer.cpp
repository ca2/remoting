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

#include "RfbServer.h"
#include "remoting/remoting_common/server_config/Configurator.h"

RfbServer::RfbServer(const ::scoped_string & scopedstrBindHost, unsigned short bindPort,
                     RfbClientManager *clientManager,
                     bool lockAddr,
                     LogWriter *log,
                     const ::int_rectangle & viewPort)
: TcpServer(bindHost, bindPort, false, lockAddr),
  m_clientManager(clientManager),
  m_log(log)
{
  if (viewPort != 0) {
    m_viewPort.setArbitraryRect(viewPort);
  }

  TcpServer::start();

  if (viewPort == 0) {
    m_log->debug("Rfb server started at {}:{}", bindHost, (int)bindPort);
  } else {
    m_log->debug("Rfb server started at {}:{} with [{} {} {} {}] view port specified",
                 bindHost, (int)bindPort,
                 viewPort.left, viewPort.right, viewPort.top, viewPort.bottom);
  }
}

RfbServer::~RfbServer()
{
  m_log->debug("Rfb server at {}:{} stopped", getBindHost(), (int)getBindPort());
}

void RfbServer::onAcceptConnection(SocketIPv4 *socket)
{
  try {
    // Get incoming connection address and convert it to string.
    SocketAddressIPv4 peerAddr;
    socket->getPeerAddr(&peerAddr);
    ::string peerIpString;
    peerAddr.toString(&peerIpString);

    m_log->debug("Incoming rfb connection from {} to port %u", peerIpString, peerAddr.getPort());

    struct sockaddr_in addr_in = peerAddr.getSockAddr();

    // Check access control rules for the IP address of the peer.
    // FIXME: Check loopback-related rules separately, report differently.
    ServerConfig *config = Configurator::getInstance()->getServerConfig();
    IpAccessRule::ActionType action = config->getActionByAddress((unsigned long)addr_in.sin_addr.S_un.S_addr);

    if (action == IpAccessRule::ACTION_TYPE_DENY) {
      m_log->debug("Connection rejected due to access control rules");
      delete socket;
      return;
    }

    // Access granted, add new RFB client. One more check will follow later in
    // RfbClientManager::onCheckAccessControl().

    socket->enableNaggleAlgorithm(false);

    m_clientManager->addNewConnection(socket, &m_viewPort, false, false);

  } catch (::remoting::Exception &ex) {
    m_log->error("Failed to process incoming rfb connection with following reason: \"{}\"", ex.get_message());
  }
}
