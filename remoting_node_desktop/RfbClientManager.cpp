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

#include "RfbClientManager.h"
#include "thread/ZombieKiller.h"
#include "QueryConnectionApplication.h"
#include "server_config_lib/Configurator.h"
#include "util/MemUsage.h"

RfbClientManager::RfbClientManager(const ::scoped_string & scopedstrServerName,
                                   NewConnectionEvents *newConnectionEvents,
                                   LogWriter *log,
                                   DesktopFactory *desktopFactory)
: m_nextClientId(0),
  m_desktop(0),
  m_newConnectionEvents(newConnectionEvents),
  m_log(log),
  m_desktopFactory(desktopFactory)
{
  m_log->information("Starting rfb client manager");
}

RfbClientManager::~RfbClientManager()
{
  m_log->information("~RfbClientManager() has been called");
  disconnectAllClients();
  waitUntilAllClientAreBeenDestroyed();
  m_log->information("~RfbClientManager() has been completed");
}

void RfbClientManager::onClientTerminate()
{
  validateClientList();
}

Desktop *RfbClientManager::onClientAuth(RfbClient *client)
{
  // The client is now authenticated, so remove its IP from the ban ::list_base.
  ::string ip;
  client->getPeerHost(&ip);
  updateIpInBan(&ip, true);

  m_newConnectionEvents->onSuccAuth(&ip);

  AutoLock al(&m_clientListLocker);

  // Checking if this client is allowed to connect, depending on its "shared"
  // flag and the server's configuration.
  ServerConfig *servConf = Configurator::getInstance()->getServerConfig();
  bool isAlwaysShared = servConf->isAlwaysShared();
  bool isNeverShared = servConf->isNeverShared();

  bool isResultShared;
  if (isAlwaysShared) {
    isResultShared = true;
  } else if (isNeverShared) {
    isResultShared = false;
  } else {
    isResultShared = client->getSharedFlag();
  }

  // If the client wishes to have exclusive access then remove other clients.
  if (!isResultShared) {
    // Which client takes priority, existing or incoming?
    if (servConf->isDisconnectingExistingClients()) {
      // Incoming
      disconnectAuthClients();
    } else {
      // Existing
      if (!m_clientList.empty()) {
        throw ::remoting::Exception("Cannot disconnect existing clients and therefore"
                        " the client will be disconected"); // Disconnect this client
      }
    }
  }

  // Removing the client from the non-authorized clients ::list_base.
  for (ClientListIter iter = m_nonAuthClientList.begin();
       iter != m_nonAuthClientList.end(); iter++) {
    RfbClient *clientOfList = *iter;
    if (clientOfList == client) {
      m_nonAuthClientList.erase(iter);
      break;
    }
  }

  // Adding to the authorized ::list_base.
  m_clientList.add(client);

  if (m_desktop == 0 && !m_clientList.empty()) {
    // Create WinDesktop and notify listeners that the first client has been
    // connected.
    m_desktop = m_desktopFactory->createDesktop(this, this, this, m_log);
    ::std::vector<RfbClientManagerEventListener *>::iterator iter;
    for (iter = m_listeners.begin(); iter != m_listeners.end(); iter++) {
      (*iter)->afterFirstClientConnect();
    }
  }
  return m_desktop;
}

bool RfbClientManager::onCheckForBan(RfbClient *client)
{
  ::string ip;
  client->getPeerHost(&ip);

  return checkForBan(&ip);
}

void RfbClientManager::onAuthFailed(RfbClient *client)
{
  ::string ip;
  client->getPeerHost(&ip);

  updateIpInBan(&ip, false);

  m_newConnectionEvents->onAuthFailed(&ip);
}

void RfbClientManager::onCheckAccessControl(RfbClient *client)
{
  SocketAddressIPv4 peerAddr;

  try {
    client->getSocketAddr(&peerAddr);
  } catch (...) {
    throw AuthException("Failed to get IP address of the RFB client");
  }

  struct sockaddr_in addr_in = peerAddr.getSockAddr();

  ServerConfig *config = Configurator::getInstance()->getServerConfig();

  IpAccessRule::ActionType action;

  if (!client->isOutgoing()) {
    action = config->getActionByAddress((unsigned long)addr_in.sin_addr.S_un.S_addr);
  } else {
    action = IpAccessRule::ACTION_TYPE_ALLOW;
  }

  // Promt user to know what to do with incmoing connection.

  if (action == IpAccessRule::ACTION_TYPE_QUERY) {
    ::string peerHost;

    peerAddr.toString(&peerHost);

    int queryRetVal = QueryConnectionApplication::execute(peerHost,
                                                          config->isDefaultActionAccept(),
                                                          config->getQueryTimeout());
    if (queryRetVal == 1) {
      throw AuthException("Connection has been rejected");
    }
  }
}

void RfbClientManager::onClipboardUpdate(const ::scoped_string & newClipboard)
{
  AutoLock al(&m_clientListLocker);
  for (ClientListIter iter = m_clientList.begin();
       iter != m_clientList.end(); iter++) {
    if ((*iter)->getClientState() == IN_NORMAL_PHASE) {
      (*iter)->sendClipboard(newClipboard);
    }
  }
}

void RfbClientManager::onSendUpdate(const UpdateContainer *updateContainer,
                                    const CursorShape *cursorShape)
{
  AutoLock al(&m_clientListLocker);
  for (ClientListIter iter = m_clientList.begin();
       iter != m_clientList.end(); iter++) {
    if ((*iter)->getClientState() == IN_NORMAL_PHASE) {
      (*iter)->sendUpdate(updateContainer, cursorShape);
    }
  }
}

bool RfbClientManager::isReadyToSend()
{
  AutoLock al(&m_clientListLocker);
  bool isReady = false;
  for (ClientListIter iter = m_clientList.begin();
       iter != m_clientList.end(); iter++) {
    if ((*iter)->getClientState() == IN_NORMAL_PHASE) {
      isReady = isReady || (*iter)->clientIsReady();
    }
  }
  return isReady;
}

void RfbClientManager::onAbnormalDesktopTerminate()
{
  m_log->error("onAbnormalDesktopTerminate() called");
  disconnectAllClients();
}

void RfbClientManager::disconnectAllClients()
{
  AutoLock al(&m_clientListLocker);
  disconnectNonAuthClients();
  disconnectAuthClients();
}

void RfbClientManager::disconnectNonAuthClients()
{
  AutoLock al(&m_clientListLocker);
  for (ClientListIter iter = m_nonAuthClientList.begin();
       iter != m_nonAuthClientList.end(); iter++) {
    (*iter)->disconnect();
  }
}

void RfbClientManager::disconnectAuthClients()
{
  AutoLock al(&m_clientListLocker);
  for (ClientListIter iter = m_clientList.begin();
       iter != m_clientList.end(); iter++) {
    (*iter)->disconnect();
  }
}

void RfbClientManager::waitUntilAllClientAreBeenDestroyed()
{
  while (true) {
    {
      AutoLock al(&m_clientListLocker);
      if (m_clientList.empty() && m_nonAuthClientList.empty()) {
        break;
      }
    }
    m_listUnderflowingEvent.waitForEvent();
  }
  ZombieKiller::getInstance()->killAllZombies();
}

void RfbClientManager::validateClientList()
{
  Desktop *objectToDestroy = 0;
  {
    AutoLock al(&m_clientListLocker);
    // If clients are in the IN_READY_TO_REMOVE phase, remove them from the
    // non-authorized clients ::list_base.
    ClientListIter iter = m_nonAuthClientList.begin();
    while (iter != m_nonAuthClientList.end()) {
      RfbClient *client = *iter;
      ClientState state = client->getClientState();
      if (state == IN_READY_TO_REMOVE) {
        iter = m_nonAuthClientList.erase(iter);
        ZombieKiller::getInstance()->addZombie(client);
      } else {
        iter++;
      }
    }
    // If clients are in the IN_READY_TO_REMOVE phase, remove them from the
    // authorized clients ::list_base.
    iter = m_clientList.begin();
    while (iter != m_clientList.end()) {
      RfbClient *client = *iter;
      ClientState state = client->getClientState();
      if (state == IN_READY_TO_REMOVE) {
        iter = m_clientList.erase(iter);
        ZombieKiller::getInstance()->addZombie(client);
      } else {
        iter++;
      }
    }

    if (m_desktop != 0 && m_clientList.empty()) {
      objectToDestroy = m_desktop;
      m_desktop = 0;
    }
  }
  if (objectToDestroy != 0) {
    delete objectToDestroy;
    ::std::vector<RfbClientManagerEventListener *>::iterator iter;
    for (iter = m_listeners.begin(); iter != m_listeners.end(); iter++) {
      (*iter)->afterLastClientDisconnect();
    }
  }

  AutoLock al(&m_clientListLocker);
  if (m_clientList.empty() && m_nonAuthClientList.empty()) {
    m_listUnderflowingEvent.notify();
  }
}

bool RfbClientManager::checkForBan(const ::scoped_string & ip)
{
  AutoLock al(&m_banListMutex);

  BanListIter it = m_banList.find(*ip);
  if (it != m_banList.end()) {
    unsigned int count = (*it).second.count;
    ::earth::time lastTime = (*it).second.banLastTime;
    ::earth::time now = ::earth::time::now();
    if (count > 13) count = 13; 
    // about 1 hour max login rate after 14 unsuccessful logins
    // wait about 1 minute after 8 unsuccessful logins
    if ((now-lastTime).getTime() < 500 * (1 << count)) {  
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void RfbClientManager::updateIpInBan(const ::scoped_string & ip, bool success)
{
  AutoLock al(&m_banListMutex);

  BanListIter it = m_banList.find(*ip);
  if (success) {
    if (it != m_banList.end()) {
      // Even if client is already banned!
      m_banList.erase(it);
    }
  } else {
    if (it != m_banList.end()) {
      // Increase ban count
      (*it).second.count += 1;
      (*it).second.banLastTime = ::earth::time::now();
    } else {
      // Add new element to ban ::list_base with ban count == 0
      BanProp banProp;
      banProp.banLastTime = ::earth::time::now();
      banProp.count = 0;
      m_banList[*ip] = banProp;
    }
  }
}

::string RfbClientManager::getBanListString()
{
  ::string str;
  for (BanListIter it = m_banList.begin(); it != m_banList.end(); it++) {
    ::string ip = (*it).first;
    ::string s;
    unsigned int count = (*it).second.count;
    ::earth::time lastTime = (*it).second.banLastTime;
    ::string time;
    lastTime.toString(&time);
    s.formatf("IP: {}, count: {}, last time: {}\n", ip, count, time);
    str.appendString(s);
  }
  return str;
}

void RfbClientManager::addNewConnection(SocketIPv4 *socket,
                                        ViewPortState *constViewPort,
                                        bool viewOnly, bool isOutgoing)
{
  AutoLock al(&m_clientListLocker);

  ServerConfig *config = Configurator::getInstance()->getServerConfig();
  int timeout = 1000 * config->getIdleTimeout();

  m_log->error("Set socket idle timeout, {} ms", timeout);

  try { 
    socket->setSocketOptions(SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    socket->setSocketOptions(SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
  } catch ( SocketException ){
    m_log->error("Can't set socket timeout, error: {}", WSAGetLastError());
  }

  _ASSERT(constViewPort != 0);

  m_log->error("Client #{} connected", m_nextClientId);
  m_log->debug("new client, process memory usage: {} ", MemUsage::getCurrentMemUsage());

  m_nonAuthClientList.add(new RfbClient(m_newConnectionEvents,
                                              socket, this, this, viewOnly,
                                              isOutgoing,
                                              m_nextClientId,
                                              constViewPort,
                                              &m_dynViewPort,
                                              timeout,
                                              m_log));
  m_nextClientId++;
}

void RfbClientManager::getClientsInfo(RfbClientInfoList *::list_base)
{
  AutoLock al(&m_clientListLocker);

  for (ClientListIter it = m_clientList.begin(); it != m_clientList.end(); it++) {
    RfbClient *each = *it;
    if (each->getClientState() == IN_NORMAL_PHASE) {
      ::string peerHost;

      each->getPeerHost(&peerHost);

      ::list_base->add(RfbClientInfo(each->getId(), peerHost));
    }
  }
}

void RfbClientManager::setDynViewPort(const ViewPortState *dynViewPort)
{
  AutoLock al(&m_clientListLocker);
  m_dynViewPort = *dynViewPort;

  // Assign the dynViewPort value for all already run clients too.
  for (ClientListIter iter = m_clientList.begin();
       iter != m_clientList.end(); iter++) {
    (*iter)->changeDynViewPort(dynViewPort);
  }
  for (ClientListIter iter = m_nonAuthClientList.begin();
       iter != m_nonAuthClientList.end(); iter++) {
    (*iter)->changeDynViewPort(dynViewPort);
  }
}
