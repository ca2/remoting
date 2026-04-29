// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopClientImpl.h"
#include "remoting/remoting/node_config/Configurator.h"
#include "remoting/remoting/desktop_ipc/UpdateHandlerClient.h"
#include "windows/WindowsInputBlocker.h"
#include "remoting/remoting/desktop_ipc/UserInputClient.h"
#include "SasUserInput.h"
#include "windows/WindowsUserInput.h"
#include "DesktopConfigLocal.h"

namespace remoting_windows
{

   DesktopClientImpl::DesktopClientImpl(ClipboardListener *pclipboardlistenerExternal,
                                        UpdateSendingListener *pupdatesendinglistenerExternal,
                                        AbnormDeskTermListener *pdesktermlistenerExternal, ::subsystem::LogWriter * plogwriter) :
       DesktopBaseImpl(pclipboardlistenerExternal, pupdatesendinglistenerExternal, pdesktermlistenerExternal, log), m_pchannelClientToServer(0),
       m_pchannelServerToClient(0), m_pgateClientToServer(0), m_pgateServerToClient(0), m_pdesktopserverwatcher(0), m_pdesktopsrvdispatcher(0),
       m_puserinput(0), m_pdesktopconfigclient(0), m_pgatekicker(0), m_plogwriter = plogwriter;
   {
      m_plogwriter->information("Creating DesktopClientImpl");

      try
      {
         m_plogwriter->debug("DesktopClientImpl: Try to initialize DesktopServerWatcher");
         m_pdesktopserverwatcher = new DesktopServerWatcher(this, m_plogwriter);

         // Transport initialization
         m_plogwriter->debug("DesktopClientImpl: Initializing ReconnectingChannel(s)...");
         m_pchannelClientToServer = new ReconnectingChannel(60000, m_plogwriter);
         m_pchannelServerToClient = new ReconnectingChannel(60000, m_plogwriter);

         // At this point the all DesktopServerWatcher's callback resources is initialized.
         m_plogwriter->debug("DesktopClientImpl: Resuming DesktopServerWatcher");
         m_pdesktopserverwatcher->resume();

         m_plogwriter->debug("DesktopClientImpl: Creating BlockingGate wrappers for the ReconnectingChannel(s)");
         m_pgateClientToServer = new BlockingGate(m_pchannelClientToServer);
         m_pgateServerToClient = new BlockingGate(m_pchannelServerToClient);

         m_plogwriter->debug("DesktopClientImpl: Initializing DesktopSrvDispatcher");
         m_pdesktopsrvdispatcher = new DesktopSrvDispatcher(m_pgateServerToClient, this, m_plogwriter);

         m_plogwriter->debug("DesktopClientImpl: Initializing UpdateHandlerClient...");
         m_pupdatehandler = new UpdateHandlerClient(m_pgateClientToServer, m_pdesktopsrvdispatcher, this, m_plogwriter);

         m_plogwriter->debug("DesktopClientImpl: Initializing UserInputClient...");
         UserInputClient *userInputClient = new UserInputClient(m_pgateClientToServer, m_pdesktopsrvdispatcher, this);
         m_puserinput = userInputClient;
         m_plogwriter->debug("DesktopClientImpl: Initializing SasUserInput...");
         m_puserinput = new SasUserInput(userInputClient, m_plogwriter);

         m_plogwriter->debug("DesktopClientImpl: Initializing DesktopConfigClient...");
         m_pdesktopconfigclient = new DesktopConfigClient(m_pgateClientToServer);
         m_plogwriter->debug("DesktopClientImpl: Initializing GateKicker...");
         m_pgatekicker = new GateKicker(m_pgateClientToServer);
         // Start dispatcher after handler registrations
         m_plogwriter->debug("DesktopClientImpl: Resuming DesktopSrvDispatcher");
         m_pdesktopsrvdispatcher->resume();
         m_plogwriter->debug("DesktopClientImpl: Calling onConfigReload(0)");
         onConfigReload(0);

         m_plogwriter->debug("DesktopClientImpl: Registering as a listener in the Configurator");
         m_pconfigurator->addListener(this);
      }
      catch (::subsystem::Exception &ex)
      {
         m_plogwriter->error("::subsystem::Exception during DesktopClientImpl creaion: {}", ex.get_message());
         freeResource();
         throw;
      }
      m_plogwriter->debug("DesktopClientImpl: Resuming self thread");
      resume();
   }

   DesktopClientImpl::~DesktopClientImpl()
   {
      m_plogwriter->information("Deleting DesktopClientImpl");
      terminate();
      wait();
      freeResource();
      m_plogwriter->information("DesktopClientImpl deleted");
   }

   void DesktopClientImpl::freeResource()
   {
      m_pconfigurator->removeListener(this);

      if (m_pdesktopserverwatcher)
         delete m_pdesktopserverwatcher;

      closeDesktopServerTransport();

      if (m_pdesktopsrvdispatcher)
         delete m_pdesktopsrvdispatcher;

      if (m_pgatekicker)
         delete m_pgatekicker;
      if (m_pupdatehandler)
         delete m_pupdatehandler;
      if (m_pdesktopconfigclient)
         delete m_pdesktopconfigclient;
      if (m_puserinput)
         delete m_puserinput;
      if (m_puserinput)
         delete m_puserinput;

      if (m_pgateServerToClient)
         delete m_pgateServerToClient;
      if (m_pgateClientToServer)
         delete m_pgateClientToServer;

      if (m_pchannelServerToClient)
         delete m_pchannelServerToClient;
      if (m_pchannelClientToServer)
         delete m_pchannelClientToServer;
   }

   void DesktopClientImpl::closeDesktopServerTransport()
   {
      try
      {
         if (m_pchannelClientToServer)
            m_pchannelClientToServer->close();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Cannot close client->server channel from Windesktop: {}", e.get_message());
      }
      try
      {
         if (m_pchannelServerToClient)
            m_pchannelServerToClient->close();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Cannot close server->client channel from Windesktop: {}", e.get_message());
      }
   }

   void DesktopClientImpl::onAnObjectEvent()
   {
      m_pdesktermlistenerExternal->onAbnormalDesktopTerminate();
      m_plogwriter->error("Forced closing of pipe conections");
      closeDesktopServerTransport();
   }

   void DesktopClientImpl::onReconnect(Channel *newChannelTo, Channel *newChannelFrom)
   {
      BlockingGate pblockinggate(newChannelTo);
      if (m_pdesktopconfigclient)
      {
         m_plogwriter->information("try update remote configuration from the "
                                   "DesktopClientImpl::onReconnect() function");
         m_pdesktopconfigclient->updateByNewSettings(&pblockinggate);
      }
      if (m_pupdatehandler)
      {
         m_plogwriter->information("try update remote UpdateHandler from the "
                                   "DesktopClientImpl::onReconnect() function");
         m_pupdatehandler->sendInit(&pblockinggate);
      }
      if (m_puserinput)
      {
         m_plogwriter->information("try update remote UserInput from the "
                                   "DesktopClientImpl::onReconnect() function");
         m_puserinput->sendInit(&pblockinggate);
      }

      m_pchannelClientToServer->replaceChannel(newChannelTo);
      m_pchannelServerToClient->replaceChannel(newChannelFrom);
   }

   void DesktopClientImpl::onTerminate() { m_happeningNewUpdate.set_happening(); }

   void DesktopClientImpl::execute()
   {
      m_plogwriter->information("DesktopClientImpl thread started");

      while (!isTerminating())
      {
         m_happeningNewUpdate.wait();
         if (!isTerminating())
         {
            sendUpdate();
         }
      }

      m_plogwriter->information("DesktopClientImpl thread stopped");
   }

   bool DesktopClientImpl::isRemoteInputTempBlocked() { return !m_pdesktopconfigclient->isRemoteInputAllowed(); }

   void DesktopClientImpl::applyNewConfiguration()
   {
      m_plogwriter->information("reload DesktopClientImpl configuration");
      m_pdesktopconfigclient->updateByNewSettings(m_pgateClientToServer);
   }


} // namespace remoting_windows


