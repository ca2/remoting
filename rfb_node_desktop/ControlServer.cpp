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
#include "ControlServer.h"
#include "ControlClient.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "remoting/rfb_node_desktop/control_desktop/NamedPipeTransport.h"


namespace remoting_rfb_node_desktop
{


   ControlServer::ControlServer(::remoting_rfb_node::Configurator * pconfigurator, ::subsystem::PipeServer *pipeServer, RfbClientManager *rfbClientManager,
                                ::subsystem::LogWriter * plogwriter) : Thread("CtrlSvr"),
       m_pconfigurator(pconfigurator),m_ppipeserver(pipeServer), m_prfbclientmanager(rfbClientManager), m_plogwriter(plogwriter)
   {
      constructø(m_pthreadCollector);
      m_pcontrolappauthenticator = allocateø ControlAppAuthenticator(30_s, 3);
      m_plogwriter->debug("{}", "::innate_subsystem::Control server started");

      resumeThread();
      
   }


   ControlServer::~ControlServer()
   {
      m_plogwriter->debug("Destroying control server transport");

      //setThreadToFinish();
      //waitThreadToFinish();

      try
      {
         m_ppipeserver->close();
      }
      catch (::subsystem::Exception &ex)
      {
         m_plogwriter->error("Failed to destroy control server transport with '{}' reason", ex.get_message());
      }

      //delete m_ppipeserver;

      // Unblock all client if it has been blocked by authenticator
      m_pcontrolappauthenticator->breakAndDisableAuthentications();

      m_plogwriter->debug("{}"), "::innate_subsystem::Control server stopped";
   }

   void ControlServer::onThreadMain()
   {
      try
      {
         while (!isThreadTerminating())
         {
            auto ppipe = m_ppipeserver->accept();
            auto ptransport = allocateø ::remoting_control_desktop::NamedPipeTransport(ppipe);

            auto pcontrolclient =
               allocateø ControlClient(m_pconfigurator, ptransport, m_prfbclientmanager, m_pcontrolappauthenticator, ppipe->getFile(), m_plogwriter);

            pcontrolclient->resumeThread();

            m_pthreadCollector->addThread(pcontrolclient);
         }
      }
      catch (::subsystem::Exception &ex)
      {
         m_plogwriter->error("::subsystem::Exception on control server thread: {}", ex.get_message());
      }
   }

   void ControlServer::onTermThread()
   {
      try
      {
         m_ppipeserver->close();
      }
      catch (...)
      {
      }
   }


} // namespace remoting_rfb_node_desktop
