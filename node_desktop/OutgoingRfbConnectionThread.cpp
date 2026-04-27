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
#include "OutgoingRfbConnectionThread.h"
#include "subsystem/socket/SocketIPv4.h"

namespace remoting_node_desktop
{


   OutgoingRfbConnectionThread::OutgoingRfbConnectionThread(const ::scoped_string &scopedstrConnectHost,
                                                            unsigned int connectPort, bool viewOnly,
                                                            RfbClientManager *clientManager,
                                                            ::subsystem::LogWriter * plogwriter) :
       m_connectHost(scopedstrConnectHost), m_connectPort(connectPort), m_viewOnly(viewOnly),
       m_clientManager(clientManager), m_plogwriter(plogwriter)
   {
   }

   OutgoingRfbConnectionThread::~OutgoingRfbConnectionThread() {}

   void OutgoingRfbConnectionThread::execute()
   {
      auto psocket = createø<::subsystem::SocketIPv4Interface>();

      try
      {
         psocket->connect(m_connectHost, m_connectPort);
      }
      catch (::subsystem::Exception &someEx)
      {
         m_plogwriter->error("Failed to connect to {}:{} with reason: '{}'", m_connectHost, m_connectPort,
                             someEx.get_message());
         // delete socket;
         return;
      }

      ::remoting::ViewPortState viewportstate;

      m_clientManager->addNewConnection(psocket,
                                        &viewportstate, // with a default view port
                                        m_viewOnly, true);
   }


} // namespace remoting_node_desktop
