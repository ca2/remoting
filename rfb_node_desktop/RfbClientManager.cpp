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
#include "RfbClientManager.h"
//#include "subsystem/thread/ZombieKiller.h"
#include "QueryConnectionApplication.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "subsystem/platform/MemUsage.h"
#include "subsystem_bsd_sockets/socket/SocketAddressIPv4.h"
#include "subsystem/socket/SocketIPv4.h"


namespace remoting_rfb_node_desktop
{


   RfbClientManager::RfbClientManager(const ::scoped_string &scopedstrServerName,
                                      ::remoting_rfb_node::Configurator *pconfigurator,
                                      ::remoting_rfb_node::NewConnectionEvents *pnewconnectionevents,
                                      ::subsystem::LogWriter * plogwriter, ::remoting_rfb::DesktopFactory *desktopFactory) :
       m_nextClientId(0), m_pdesktop(0), m_pconfigurator(pconfigurator), m_pnewconnectionevents(pnewconnectionevents),
       m_plogwriter(plogwriter), m_desktopFactory(desktopFactory)
   {
      m_plogwriter->information("Starting rfb client manager");
   }

   RfbClientManager::~RfbClientManager()
   {
      m_plogwriter->information("~RfbClientManager() has been called");
      disconnectAllClients();
      waitUntilAllClientAreBeenDestroyed();
      m_plogwriter->information("~RfbClientManager() has been completed");
   }

   void RfbClientManager::onClientTerminate() { validateClientList(); }

   ::remoting_rfb::Desktop *RfbClientManager::onClientAuth(::remoting_rfb::RfbClient *client)
   {
      // The client is now authenticated, so remove its IP from the ban ::list_base.
      ::string ip;
      client->getPeerHost(ip);
      updateIpInBan(ip, true);

      m_pnewconnectionevents->onSuccAuth(ip);

      critical_section_lock al(&m_clientListLocker);

      // Checking if this client is allowed to connect, depending on its "shared"
      // flag and the server's configuration.
      auto servConf = m_pconfigurator->getServerConfig();
      bool isAlwaysShared = servConf->isAlwaysShared();
      bool isNeverShared = servConf->isNeverShared();

      bool isResultShared;
      if (isAlwaysShared)
      {
         isResultShared = true;
      }
      else if (isNeverShared)
      {
         isResultShared = false;
      }
      else
      {
         isResultShared = client->getSharedFlag();
      }

      // If the client wishes to have exclusive access then remove other clients.
      if (!isResultShared)
      {
         // Which client takes priority, existing or incoming?
         if (servConf->isDisconnectingExistingClients())
         {
            // Incoming
            disconnectAuthClients();
         }
         else
         {
            // Existing
            if (!m_clientList.empty())
            {
               throw ::subsystem::Exception("Cannot disconnect existing clients and therefore"
                                            " the client will be disconected"); // Disconnect this client
            }
         }
      }

      // Removing the client from the non-authorized clients ::list_base.
      for (ClientListIter iter = m_nonAuthClientList.begin(); iter != m_nonAuthClientList.end(); iter++)
      {
         ::remoting_rfb::RfbClient * clientOfList = *iter;
         if (clientOfList == client)
         {
            m_nonAuthClientList.erase(iter);
            break;
         }
      }

      // Adding to the authorized ::list_base.
      m_clientList.add(client);

      if (m_pdesktop == 0 && !m_clientList.empty())
      {
         // Create WinDesktop and notify listeners that the first client has been
         // connected.
         m_pdesktop = m_desktopFactory->createDesktop(m_pconfigurator, this, this, this, m_plogwriter);
         //::std::vector<RfbClientManagerEventListener *>::iterator iter;
         for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
         {
            (*iter)->afterFirstClientConnect();
         }
      }
      return m_pdesktop;
   }

   bool RfbClientManager::onCheckForBan(::remoting_rfb::RfbClient *client)
   {
      ::string ip;
      client->getPeerHost(ip);

      return checkForBan(ip);
   }

   void RfbClientManager::onAuthFailed(::remoting_rfb::RfbClient *client)
   {
      ::string ip;
      client->getPeerHost(ip);

      updateIpInBan(ip, false);

      m_pnewconnectionevents->onAuthFailed(ip);
   }


   void RfbClientManager::onCheckAccessControl(::remoting_rfb::RfbClient *client)
   {

      ::pointer<::subsystem::SocketAddressIPv4Interface> peerAddr;

      try
      {
         peerAddr = client->getSocketAddr();
      }
      catch (...)
      {
         throw ::remoting_rfb_node::AuthException("Failed to get IP address of the RFB client");
      }

      auto paddrImpl = peerAddr->impl<::subsystem_bsd_sockets::SocketAddressIPv4>();
      // struct sockaddr_in addr_in = peerAddr.getSockAddr();
      struct sockaddr_in addr_in = paddrImpl->_getSockAddr();

      ::remoting_rfb_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();

      ::remoting_rfb_node::IpAccessRule::ActionType action;

      if (!client->isOutgoing())
      {
         
#ifdef WINDOWS
         action = pserverconfig->getActionByAddress((unsigned long)addr_in.sin_addr.S_un.S_addr);
#else
         action = pserverconfig->getActionByAddress((unsigned long)addr_in.sin_addr.s_addr);
#endif
      }
      else
      {
         action =::remoting_rfb_node:: IpAccessRule::ACTION_TYPE_ALLOW;
      }

      // Promt user to know what to do with incmoing connection.

      if (action == ::remoting_rfb_node::IpAccessRule::ACTION_TYPE_QUERY)
      {
         ::string peerHost;

         peerHost = peerAddr->toString();

         auto papp = create_newø<::remoting_node_desktop::QueryConnectionApplication>();

         auto queryRetVal = papp->run_synchronously(peerHost, pserverconfig->isDefaultActionAccept(), pserverconfig->getQueryTimeout());

         // int queryRetVal = QueryConnectionApplication::onThreadMain(
         //    peerHost,
         //    pserverconfig->isDefaultActionAccept(),
         //    pserverconfig->getQueryTimeout());
         if (queryRetVal == 1)
         {
            throw ::remoting_rfb_node::AuthException("Connection has been rejected");
         }
      }
   }

   void RfbClientManager::onClipboardUpdate(const ::scoped_string &newClipboard)
   {
      critical_section_lock al(&m_clientListLocker);
      for (ClientListIter iter = m_clientList.begin(); iter != m_clientList.end(); iter++)
      {
         if ((*iter)->getClientState() == ::remoting_rfb::IN_NORMAL_PHASE)
         {
            (*iter)->sendClipboard(newClipboard);
         }
      }
   }

   void RfbClientManager::onSendUpdate(const ::remoting_rfb::UpdateContainer & updatecontainer,
                                       const ::remoting_rfb::CursorShape *cursorShape)
   {
      critical_section_lock al(&m_clientListLocker);
      for (ClientListIter iter = m_clientList.begin(); iter != m_clientList.end(); iter++)
      {
         if ((*iter)->getClientState() == ::remoting_rfb::IN_NORMAL_PHASE)
         {
            (*iter)->sendUpdate(updatecontainer, cursorShape);
         }
      }
   }

   bool RfbClientManager::isReadyToSend()
   {
      critical_section_lock al(&m_clientListLocker);
      bool isReady = false;
      for (ClientListIter iter = m_clientList.begin(); iter != m_clientList.end(); iter++)
      {
         if ((*iter)->getClientState() == ::remoting_rfb::IN_NORMAL_PHASE)
         {
            isReady = isReady || (*iter)->clientIsReady();
         }
      }
      return isReady;
   }

   void RfbClientManager::onAbnormalDesktopTerminate()
   {
      m_plogwriter->error("onAbnormalDesktopTerminate() called");
      disconnectAllClients();
   }

   void RfbClientManager::disconnectAllClients()
   {
      critical_section_lock al(&m_clientListLocker);
      disconnectNonAuthClients();
      disconnectAuthClients();
   }

   void RfbClientManager::disconnectNonAuthClients()
   {
      critical_section_lock al(&m_clientListLocker);
      for (ClientListIter iter = m_nonAuthClientList.begin(); iter != m_nonAuthClientList.end(); iter++)
      {
         (*iter)->disconnect();
      }
   }

   void RfbClientManager::disconnectAuthClients()
   {
      critical_section_lock al(&m_clientListLocker);
      for (ClientListIter iter = m_clientList.begin(); iter != m_clientList.end(); iter++)
      {
         (*iter)->disconnect();
      }
   }

   void RfbClientManager::waitUntilAllClientAreBeenDestroyed()
   {
      while (true)
      {
         {
            critical_section_lock al(&m_clientListLocker);
            if (m_clientList.empty() && m_nonAuthClientList.empty())
            {
               break;
            }
         }
         /// m_listUnderflowingEvent.waitForEvent();
         m_listUnderflowingEvent.wait();
      }
      /// ZombieKiller::getInstance()->killAllZombies();
   }

   void RfbClientManager::validateClientList()
   {
      ::remoting_rfb::Desktop *objectToDestroy = 0;
      {
         critical_section_lock al(&m_clientListLocker);
         // If clients are in the IN_READY_TO_REMOVE phase, remove them from the
         // non-authorized clients ::list_base.
         auto iter = m_nonAuthClientList.begin();
         while (iter != m_nonAuthClientList.end())
         {
            ::remoting_rfb::RfbClient *client = *iter;
            ::remoting_rfb::ClientState state = client->getClientState();
            if (state == ::remoting_rfb::IN_READY_TO_REMOVE)
            {
               iter = m_nonAuthClientList.erase(iter);
               // ZombieKiller::getInstance()->addZombie(client);
            }
            else
            {
               iter++;
            }
         }
         // If clients are in the IN_READY_TO_REMOVE phase, remove them from the
         // authorized clients ::list_base.
         iter = m_clientList.begin();
         while (iter != m_clientList.end())
         {
            ::remoting_rfb::RfbClient *client = *iter;
            ::remoting_rfb::ClientState state = client->getClientState();
            if (state == ::remoting_rfb::IN_READY_TO_REMOVE)
            {
               iter = m_clientList.erase(iter);
               // ZombieKiller::getInstance()->addZombie(client);
            }
            else
            {
               iter++;
            }
         }

         if (m_pdesktop != 0 && m_clientList.empty())
         {
            objectToDestroy = m_pdesktop;
            m_pdesktop = 0;
         }
      }
      if (objectToDestroy != 0)
      {
         delete objectToDestroy;
         //::std::vector<RfbClientManagerEventListener *>::iterator iter;
         for (auto iter = m_listeners.begin(); iter != m_listeners.end(); iter++)
         {
            (*iter)->afterLastClientDisconnect();
         }
      }

      critical_section_lock al(&m_clientListLocker);
      if (m_clientList.empty() && m_nonAuthClientList.empty())
      {
         // m_listUnderflowingEvent.notify();
         m_listUnderflowingEvent.set_happening();
      }
   }

   bool RfbClientManager::checkForBan(const ::scoped_string &ip)
   {
      critical_section_lock al(&m_banListMutex);

      auto it = m_banList.find(ip);
      if (it != m_banList.end())
      {
         ::u32 count = (*it).m_element2.count;
         class ::time lastTime = (*it).m_element2.banLastTime;
         class ::time now = ::time::now();
         if (count > 13)
            count = 13;
         // about 1 hour max login rate after 14 unsuccessful logins
         // waitThreadToFinish about 1 minute after 8 unsuccessful logins
         if ((now - lastTime).m_iSecond < 500 * (1 << count))
         {
            return true;
         }
         else
         {
            return false;
         }
      }
      else
      {
         return false;
      }
   }

   void RfbClientManager::updateIpInBan(const ::scoped_string &ip, bool success)
   {
      critical_section_lock al(&m_banListMutex);

      // BanListIter it = m_banList.find(*ip);
      auto it = m_banList.find(ip);
      if (success)
      {
         if (it != m_banList.end())
         {
            // Even if client is already banned!
            m_banList.erase(it);
         }
      }
      else
      {
         if (it != m_banList.end())
         {
            // Increase ban count
            (*it).m_element2.count += 1;
            (*it).m_element2.banLastTime.Now();
         }
         else
         {
            // Add new element to ban ::list_base with ban count == 0
            BanProp banProp;
            banProp.banLastTime.Now();
            banProp.count = 0;
            m_banList[ip] = banProp;
         }
      }
   }

   ::string RfbClientManager::getBanListString()
   {
      ::string str;
      for (auto it = m_banList.begin(); it != m_banList.end(); it++)
      {
         ::string ip = it->m_element1;
         ::string s;
         ::u32 count = it->m_element2.count;
         class ::time lastTime = it->m_element2.banLastTime;
         ::string time;

         time = MainSubsystem().toString(lastTime);

         s.format("IP: {}, count: {}, last time: {}\n", ip, count, time);

         str += s;
      }

      return str;
   }


   void RfbClientManager::addNewConnection(::subsystem::SocketIPv4Interface *psocket, ::remoting_rfb::ViewPortState *constViewPort,
                                           bool viewOnly, bool isOutgoing)
   {
      critical_section_lock al(&m_clientListLocker);

      ::remoting_rfb_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();
      int timeout = 1000 * pserverconfig->getIdleTimeout();

      m_plogwriter->error("Set socket idle timeout, {} ms", timeout);

      class ::time timeTimeout;

      timeTimeout = (1_ms * timeout);

      try
      {
         psocket->setRcvTimeO(timeTimeout);
         psocket->setSndTimeO(timeTimeout);
      }
      catch (const ::subsystem::SocketException & e)
      {
         //m_plogwriter->error("Can't set socket timeout, error: {}", WSAGetLastError());
         m_plogwriter->error("Can't set socket timeout, error: {}", e.get_message());
      }

      ASSERT(constViewPort != 0);

      m_plogwriter->error("Client #{} connected", m_nextClientId);
      m_plogwriter->debug("new client, process memory usage: {} ", MainSubsystem().getCurrentMemoryUsage());

      m_nonAuthClientList.add(allocateø ::remoting_rfb::RfbClient(m_pconfigurator, m_pnewconnectionevents, psocket, this, this, viewOnly, isOutgoing,
                                            m_nextClientId, *constViewPort, m_dynViewPort, timeout, m_plogwriter));
      m_nextClientId++;
   }


   void RfbClientManager::getClientsInfo(::remoting_control_desktop::RfbClientInfoList *plist)
   {

      critical_section_lock al(&m_clientListLocker);

      for (ClientListIter it = m_clientList.begin(); it != m_clientList.end(); it++)
      {

         ::remoting_rfb::RfbClient *each = *it;

         if (each->getClientState() == ::remoting_rfb::IN_NORMAL_PHASE)
         {
            ::string peerHost;

            each->getPeerHost(peerHost);

            plist->add(::remoting_control_desktop::RfbClientInfo(each->getId(), peerHost));
         }

      }

   }

   void RfbClientManager::setDynViewPort(const ::remoting_rfb::ViewPortState *dynViewPort)
   {
      critical_section_lock al(&m_clientListLocker);
      m_dynViewPort = *dynViewPort;

      // Assign the dynViewPort value for all already onThreadMain clients too.
      for (ClientListIter iter = m_clientList.begin(); iter != m_clientList.end(); iter++)
      {
         (*iter)->changeDynViewPort(dynViewPort);
      }
      for (ClientListIter iter = m_nonAuthClientList.begin(); iter != m_nonAuthClientList.end(); iter++)
      {
         (*iter)->changeDynViewPort(dynViewPort);
      }
   }


} // namespace remoting_rfb_node_desktop



