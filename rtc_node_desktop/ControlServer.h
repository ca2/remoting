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

#pragma once


#include "RfbClientManager.h"
#include "ControlAppAuthenticator.h"
#include "subsystem/thread/ThreadCollector.h"

#include "remoting/remoting_rtc/network/TcpServer.h"

#include "subsystem/node/PipeServer.h"

namespace remoting_node_desktop
{

   /**
    * Tcp server that listens for incoming control connections
    * and give management over these connection to ControlClient instances.
    */
   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP ControlServer : public ::subsystem::Thread
   {
   public:

      ::pointer < ::remoting_rtc_node::Configurator > m_pconfigurator;

      ::pointer<ControlAppAuthenticator> m_pcontrolappauthenticator;
      ::pointer<::subsystem::ThreadCollector> m_pthreadCollector;

      ::pointer<::subsystem::PipeServer> m_ppipeserver;
      /**
       * Active rfb client manager that used in Remoting Node.
       */
      ::pointer<RfbClientManager> m_prfbclientmanager;

      ::pointer<::subsystem::LogWriter> m_plogwriter;


      /**
       * Creates and starts control server execution.
       * @param serverTransport ready transport for control server.
       * @param rfbClientManager active Remoting rfb client manager.
       * @throws ::subsystem::Exception when fail to create server.
       * @remark control server takes ownership over server transport.
       * @remark rfbClientManager and rfbServers parameters are needed for
       * executing some of control commands and cannot be 0.
       */
      ControlServer(::remoting_rtc_node::Configurator * pconfigurator, ::subsystem::PipeServer *pipeServer, RfbClientManager *rfbClientManager,
                    ::subsystem::LogWriter * plogwriter);
      /**
       * Stops and deletes control server and deletes transport.
       */
      ~ControlServer();

   protected:
      /**
       * Inherited from Thread.
       *
       * Awaits for incoming connections.
       */
      virtual void onThreadMain();

      /**
       * Inherited from Thread.
       *
       * Forced closes transport.
       */
      virtual void onTermThread();

   ////private:
   //   ::pointer < ControlAppAuthenticator > m_pcontrolappauthenticator;
   //   ::pointer<::subsystem::ThreadCollector> m_pthreadCollector;

   //   ::pointer < ::subsystem::PipeServer > m_ppipeserver;
   //   /**
   //    * Active rfb client manager that used in Remoting Node.
   //    */
   //   ::pointer < RfbClientManager > m_prfbclientmanager;

   //   ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_node_desktop




