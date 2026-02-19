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

#include "ControlClient.h"
#include "TvnServer.h"
#include "OutgoingRfbConnectionThread.h"

#include "remoting_control_desktop/ControlProto.h"

#include "network/socket/SocketStream.h"

#include "server_config_lib/Configurator.h"

#include "util/VncPassCrypt.h"

#include "rfb/HostPath.h"

#include "win_system/WTS.h"

#include "remoting_node/resource.h"

#include <time.h>
//#include "util/::string.h"
#include "util/MemUsage.h"


const unsigned int ControlClient::REQUIRES_AUTH[] = { ControlProto::ADD_CLIENT_MSG_ID,
                                                ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID,
                                                ControlProto::GET_CONFIG_MSG_ID,
                                                ControlProto::SET_CONFIG_MSG_ID,
                                                ControlProto::SHUTDOWN_SERVER_MSG_ID,
                                                ControlProto::SHARE_PRIMARY_MSG_ID,
                                                ControlProto::SHARE_DISPLAY_MSG_ID,
                                                ControlProto::SHARE_WINDOW_MSG_ID,
                                                ControlProto::SHARE_RECT_MSG_ID,
                                                ControlProto::SHARE_APP_MSG_ID,
                                                ControlProto::SHARE_FULL_MSG_ID,
                                                ControlProto::CONNECT_TO_TCPDISP_MSG_ID };

const unsigned int ControlClient::WITHOUT_AUTH[] = {
  ControlProto::AUTH_MSG_ID,
  ControlProto::RELOAD_CONFIG_MSG_ID,
  ControlProto::GET_SERVER_INFO_MSG_ID,
  ControlProto::GET_CLIENT_LIST_MSG_ID,
  ControlProto::GET_SHOW_TRAY_ICON_FLAG,
  ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID
};

ControlClient::ControlClient(Transport *transport,
                             RfbClientManager *rfbClientManager,
                             ControlAppAuthenticator *authenticator,
                             HANDLE pipeHandle, LogWriter *log)
: m_transport(transport), m_rfbClientManager(rfbClientManager),
  m_authenticator(authenticator),
  m_tcpDispId(0),
  m_pipeHandle(pipeHandle),
  m_authReqMessageId(0),
  m_log(log),
  m_repeatAuthPassed(false)
{
  m_stream = m_transport->getIOStream();

  m_gate = new ControlGate(m_stream);

  m_authPassed = false;
}

ControlClient::~ControlClient()
{
  terminate();
  wait();

  delete m_gate;
  delete m_transport;
}

void ControlClient::execute()
{
  // Client passes authentication by default if server does not uses control authentication.
  if (!Configurator::getInstance()->getServerConfig()->isControlAuthEnabled()) {
    m_authPassed = true;
  }
  int i = 0;
  try {
    while (!isTerminating()) {
      unsigned int messageId = m_gate->readUInt32();
      unsigned int messageSize = m_gate->readUInt32();

      m_log->debug("Recieved control scopedstrMessage ID %u, size %u",
                  (unsigned int)messageId, (unsigned int)messageSize);
      if (++i % 10 == 0) {
        m_log->debug("process memory usage: {} ", MemUsage::getCurrentMemUsage());
      }
      bool requiresControlAuth = Configurator::getInstance()->getServerConfig()->isControlAuthEnabled();
      bool repeatAuthEnabled = Configurator::getInstance()->getServerConfig()->getControlAuthAlwaysChecking();

       // Check if scopedstrMessage requires TightVNC admin privilegies.
      if (requiresControlAuth) {
        for (size_t i = 0; i < sizeof(WITHOUT_AUTH) / sizeof(unsigned int); i++) {
          if (messageId == WITHOUT_AUTH[i]) {
            requiresControlAuth = false;
            break;
          }
        }
      }

      try {
        // Message requires control auth: skip scopedstrMessage body and sent reply.
        if (requiresControlAuth) {
          bool authPassed = m_authPassed;
          if (repeatAuthEnabled) {
            authPassed = authPassed && m_authReqMessageId == messageId && m_repeatAuthPassed;
          }
          m_repeatAuthPassed = false;
          if (!authPassed) {
            m_log->debug("Message requires control authentication");

            m_gate->skipBytes(messageSize);
            m_gate->writeUInt32(ControlProto::REPLY_AUTH_NEEDED);
            m_authReqMessageId = messageId;

            continue;
          }
        }

        switch (messageId) {
        case ControlProto::AUTH_MSG_ID:
          m_log->debug("::remoting::Window authentication requested");
          authMsgRcdv();
          break;
        case ControlProto::RELOAD_CONFIG_MSG_ID:
          m_log->debug("Command requested: Reload configuration");
          reloadConfigMsgRcvd();
          break;
        case ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID:
          m_log->debug("Command requested: Disconnect all clients command requested");
          disconnectAllMsgRcvd();
          break;
        case ControlProto::SHUTDOWN_SERVER_MSG_ID:
          m_log->debug("Command requested: Shutdown command requested");
          shutdownMsgRcvd();
          break;
        case ControlProto::ADD_CLIENT_MSG_ID:
          m_log->debug("Command requested: Attach listening viewer");
          addClientMsgRcvd();
          break;
        case ControlProto::CONNECT_TO_TCPDISP_MSG_ID:
          m_log->debug("Connect to a tcp dispatcher command requested");
          break;
        case ControlProto::GET_SERVER_INFO_MSG_ID:
          m_log->debug("::remoting::Window client requests server info");
          getServerInfoMsgRcvd();
          break;
        case ControlProto::GET_CLIENT_LIST_MSG_ID:
          m_log->debug("::remoting::Window client requests client ::list_base");
          getClientsListMsgRcvd();
          break;
        case ControlProto::SET_CONFIG_MSG_ID:
          m_log->debug("::remoting::Window client sends new server config");
          setServerConfigMsgRcvd();
          break;
        case ControlProto::GET_CONFIG_MSG_ID:
          m_log->debug("::remoting::Window client requests server config");
          getServerConfigMsgRcvd();
          break;
        case ControlProto::GET_SHOW_TRAY_ICON_FLAG:
          m_log->debug("::remoting::Window client requests tray icon visibility flag");
          getShowTrayIconFlagMsgRcvd();
          break;
        case ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID:
          m_log->debug("::remoting::Window client sends process ID");
          updateTvnControlProcessIdMsgRcvd();
          break;
        case ControlProto::SHARE_PRIMARY_MSG_ID:
          m_log->debug("Share primary scopedstrMessage recieved");
          sharePrimaryIdMsgRcvd();
          break;
        case ControlProto::SHARE_DISPLAY_MSG_ID:
          m_log->debug("Share display scopedstrMessage recieved");
          shareDisplayIdMsgRcvd();
          break;
        case ControlProto::SHARE_WINDOW_MSG_ID:
          m_log->debug("Share window scopedstrMessage recieved");
          shareWindowIdMsgRcvd();
          break;
        case ControlProto::SHARE_RECT_MSG_ID:
          m_log->debug("Share rect scopedstrMessage recieved");
          shareRectIdMsgRcvd();
          break;
        case ControlProto::SHARE_FULL_MSG_ID:
          m_log->debug("Share full scopedstrMessage recieved");
          shareFullIdMsgRcvd();
          break;
        case ControlProto::SHARE_APP_MSG_ID:
          m_log->debug("Share app scopedstrMessage recieved");
          shareAppIdMsgRcvd();
          break;
        default:
          m_gate->skipBytes(messageSize);
          m_log->warning("Received unsupported scopedstrMessage from control client");
          throw ControlException("Unknown command");
        } // switch (messageId).
      } catch (ControlException &controlEx) {
        m_log->error("::remoting::Exception while processing control client's request: \"{}\"",
                   controlEx.get_message());

        sendError(controlEx.get_message());
      }
    } // while
  } catch (::remoting::Exception &ex) {
    m_log->error("::remoting::Exception in control client thread: \"{}\"", ex.get_message());
  }
}

void ControlClient::onTerminate()
{
  try { m_transport->close(); } catch (...) { }
}

void ControlClient::sendError(const ::scoped_string & scopedstrMessage)
{
  m_gate->writeUInt32(ControlProto::REPLY_ERROR);
  m_gate->writeUTF8(scopedstrMessage);
}

//
// FIXME: Code duplicate (see RfbInitializer class).
//

void ControlClient::authMsgRcdv()
{
  unsigned char challenge[16];
  unsigned char response[16];

  srand((unsigned)time(0));
  for (int i = 0; i < sizeof(challenge); i++) {
    challenge[i] = rand() & 0xff;
  }

  m_gate->writeFully(challenge, sizeof(challenge));
  m_gate->readFully(response, sizeof(response));

  //
  // FIXME: Is it right to check if password is set after client
  // sent password to us.
  //

  ServerConfig *config = Configurator::getInstance()->getServerConfig();
  unsigned char cryptPassword[8];
  config->getControlPassword(cryptPassword);

  bool isAuthSucceed = m_authenticator->authenticate(cryptPassword,
                                                     challenge,
                                                     response);
  if (!isAuthSucceed) {
    sendError(StringTable::getString(IDS_INVALID_CONTROL_PASSWORD));
  } else {
    m_gate->writeUInt32(ControlProto::REPLY_OK);
    m_authPassed = true;
    m_repeatAuthPassed = true;
  }
}

void ControlClient::getClientsListMsgRcvd()
{
  unsigned int clientCount = 0;

  RfbClientInfoList clients;

  m_rfbClientManager->getClientsInfo(&clients);

  m_gate->writeUInt32(ControlProto::REPLY_OK);
  _ASSERT(clients.size() == (unsigned int)clients.size());
  m_gate->writeUInt32((unsigned int)clients.size());

  for (RfbClientInfoList::iterator it = clients.begin(); it != clients.end(); it++) {
    m_gate->writeUInt32((*it).m_id);
    m_gate->writeUTF8((*it).m_peerAddr);
  }
}

void ControlClient::getServerInfoMsgRcvd()
{
  bool acceptFlag = false;
  bool serviceFlag = false;

  ::string logPath;
  ::string statusText;

  TvnServerInfo info;

  TvnServer::getInstance()->getServerInfo(&info);

  ::string status;
  status= info.m_statusText;

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_gate->writeUInt8(info.m_acceptFlag ? 1 : 0);
  m_gate->writeUInt8(info.m_serviceFlag ? 1 : 0);
  m_gate->writeUTF8(status);
}

void ControlClient::reloadConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  Configurator::getInstance()->load();
}

void ControlClient::disconnectAllMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_rfbClientManager->disconnectAllClients();

  m_log->debug("Disconnecting clients");
  m_outgoingConnectionThreadCollector.destroyAllThreads();
}

void ControlClient::shutdownMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  TvnServer::getInstance()->generateExternalShutdownSignal();
}

void ControlClient::addClientMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  //
  // Read parameters.
  //

  ::string connectString;

  m_gate->readUTF8(&connectString);

  bool viewOnly = m_gate->readUInt8() == 1;

  //
  // Parse host and port from connection string.
  //
  ::string connectStringAnsi(&connectString);
  HostPath hp(connectStringAnsi, 5500);

  if (!hp.isValid()) {
    return;
  }

  ::string host;
  ::string ansiHost(hp.getVncHost());
  ansiHost.toStringStorage(&host);

  //
  // Make outgoing connection in separate thread.
  //
  OutgoingRfbConnectionThread *newConnectionThread =
                               new OutgoingRfbConnectionThread(host,
                                                               hp.getVncPort(), viewOnly,
                                                               m_rfbClientManager, m_log);

  newConnectionThread->resume();

//  ZombieKiller::getInstance()->addZombie(newConnectionThread);
  m_outgoingConnectionThreadCollector.addThread(newConnectionThread);
}
bool allZeroes(unsigned char p[ServerConfig::VNC_PASSWORD_SIZE]) {
  for (int i = 0; i < ServerConfig::VNC_PASSWORD_SIZE; i++) {
    if (p[i] != 0) {
      return false;
    }
  }
  return true;
}

void ControlClient::setServerConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);
  ServerConfig cfg;
  cfg.deserialize(m_gate);
  ServerConfig* old = Configurator::getInstance()->getServerConfig();
  unsigned char tmp[ServerConfig::VNC_PASSWORD_SIZE];

  if (cfg.hasPrimaryPassword()) {
    cfg.getPrimaryPassword(tmp);
    if (allZeroes(tmp)) { 
      // was not changed
      old->getPrimaryPassword(tmp);
      cfg.setPrimaryPassword(tmp);
    }
  }
  if (cfg.hasReadOnlyPassword()) {
    cfg.getReadOnlyPassword(tmp);
    if (allZeroes(tmp)) {
      // was not changed
      old->getReadOnlyPassword(tmp);
      cfg.setReadOnlyPassword(tmp);
    }
  }
  if (cfg.hasControlPassword()) {
    cfg.getControlPassword(tmp);
    if (allZeroes(tmp)) {
      // was not changed
      old->getControlPassword(tmp);
      cfg.setControlPassword(tmp);
    }
  }
  *old = cfg;
  Configurator::getInstance()->save();
  Configurator::getInstance()->load();
}

void ControlClient::getShowTrayIconFlagMsgRcvd()
{
  bool showIcon = Configurator::getInstance()->getServerConfig()->getShowTrayIconFlag();

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_gate->writeUInt8(showIcon ? 1 : 0);
}

void ControlClient::updateTvnControlProcessIdMsgRcvd()
{
  m_gate->readUInt32();

  try {
    WTS::duplicatePipeClientToken(m_pipeHandle);
  } catch (::exception &e) {
    m_log->error("Can't update the control client impersonation token: {}",
               e.get_message());
  }
  m_gate->writeUInt32(ControlProto::REPLY_OK);
}

void ControlClient::getServerConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ServerConfig cfg = *Configurator::getInstance()->getServerConfig();

  unsigned char zeroes[ServerConfig::VNC_PASSWORD_SIZE] = {};
  if (cfg.hasControlPassword())
    cfg.setControlPassword(zeroes);
  if (cfg.hasPrimaryPassword())
    cfg.setPrimaryPassword(zeroes);
  if (cfg.hasReadOnlyPassword())
    cfg.setReadOnlyPassword(zeroes);

  cfg.serialize(m_gate);
}

void ControlClient::sharePrimaryIdMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);
  ViewPortState dynViewPort;
  dynViewPort.setPrimaryDisplay();
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareDisplayIdMsgRcvd()
{
  unsigned char displayNumber = m_gate->readUInt8();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setDisplayNumber(displayNumber);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareWindowIdMsgRcvd()
{
  ::string windowName;
  m_gate->readUTF8(&windowName);

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setWindowName(&windowName);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareRectIdMsgRcvd()
{
  ::int_rectangle shareRect;
  shareRect.left = m_gate->readInt32();
  shareRect.top = m_gate->readInt32();
  shareRect.right = m_gate->readInt32();
  shareRect.bottom = m_gate->readInt32();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setArbitraryRect(&shareRect);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareFullIdMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setFullDesktop();
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareAppIdMsgRcvd()
{
  unsigned int procId = m_gate->readUInt32();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setProcessId(procId);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}
