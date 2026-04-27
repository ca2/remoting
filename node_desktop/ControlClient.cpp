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
#include "ControlClient.h"
#include "Server.h"
#include "OutgoingRfbConnectionThread.h"

#include "remoting/control_desktop/ControlProto.h"
#include "application.h"
//#include "remoting/remoting/network/socket/SocketStream.h"

#include "remoting/remoting/server_config/Configurator.h"
#include "remoting/remoting/server_config/ServerConfig.h"

#include "subsystem/platform/VncPassCrypt.h"
#include "subsystem_windows/platform/subsystem.h"
#include "subsystem_windows/platform/subsystem.h"
#include "subsystem/node/File.h"
#include "remoting/remoting/rfb/HostPath.h"

#include "subsystem_windows/node/WTS.h"

#include "resource.h"

//#include aaa_<time.h>
//#include "subsystem/platform/::string.h"
#include "subsystem/platform/MemUsage.h"

namespace remoting_node_desktop
{


   const unsigned int ControlClient::REQUIRES_AUTH[] = {
      ControlProto::ADD_CLIENT_MSG_ID,      ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID,
      ControlProto::GET_CONFIG_MSG_ID,      ControlProto::SET_CONFIG_MSG_ID,
      ControlProto::SHUTDOWN_SERVER_MSG_ID, ControlProto::SHARE_PRIMARY_MSG_ID,
      ControlProto::SHARE_DISPLAY_MSG_ID,   ControlProto::SHARE_WINDOW_MSG_ID,
      ControlProto::SHARE_RECT_MSG_ID,      ControlProto::SHARE_APP_MSG_ID,
      ControlProto::SHARE_FULL_MSG_ID,      ControlProto::CONNECT_TO_TCPDISP_MSG_ID};

   const unsigned int ControlClient::WITHOUT_AUTH[] = {ControlProto::AUTH_MSG_ID,
                                                       ControlProto::RELOAD_CONFIG_MSG_ID,
                                                       ControlProto::GET_SERVER_INFO_MSG_ID,
                                                       ControlProto::GET_CLIENT_LIST_MSG_ID,
                                                       ControlProto::GET_SHOW_TRAY_ICON_FLAG,
                                                       ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID};

   ControlClient::ControlClient(::remoting::Configurator * pconfigurator, Transport *transport, RfbClientManager *rfbClientManager,
                                ControlAppAuthenticator *authenticator, ::subsystem::FileInterface *pfilePipeHandle,
                                ::subsystem::LogWriter * plogwriter) :
       m_pconfigurator(pconfigurator), m_transport(transport), m_rfbClientManager(rfbClientManager), m_controlappauthenticator(authenticator), m_tcpDispId(0),
       m_pfilePipeHandle(pfilePipeHandle), m_authReqMessageId(0), m_plogwriter(plogwriter), m_repeatAuthPassed(false)
   {
      m_stream = m_transport->getIOStream();

      m_pblockinggate = new ControlGate(m_stream);

      m_authPassed = false;
   }

   ControlClient::~ControlClient()
   {
      terminate();
      wait();

      delete m_pblockinggate;
      delete m_transport;
   }

   void ControlClient::execute()
   {
      // Client passes authentication by default if server does not uses control authentication.
      if (!m_pconfigurator->getServerConfig()->isControlAuthEnabled())
      {
         m_authPassed = true;
      }
      int i = 0;
      try
      {
         while (!isTerminating())
         {
            unsigned int messageId = m_pblockinggate->readUInt32();
            unsigned int messageSize = m_pblockinggate->readUInt32();

            m_plogwriter->debug("Recieved control scopedstrMessage ID %u, size %u", (unsigned int)messageId,
                                (unsigned int)messageSize);
            if (++i % 10 == 0)
            {

               m_plogwriter->debug("process memory usage: {} ", MainSubsystem().getCurrentMemoryUsage());
            }
            bool requiresControlAuth = m_pconfigurator->getServerConfig()->isControlAuthEnabled();
            bool repeatAuthEnabled = m_pconfigurator->getServerConfig()->getControlAuthAlwaysChecking();

            // Check if scopedstrMessage requires TightVNC admin privilegies.
            if (requiresControlAuth)
            {
               for (size_t i = 0; i < sizeof(WITHOUT_AUTH) / sizeof(unsigned int); i++)
               {
                  if (messageId == WITHOUT_AUTH[i])
                  {
                     requiresControlAuth = false;
                     break;
                  }
               }
            }

            try
            {
               // Message requires control auth: skip scopedstrMessage body and sent reply.
               if (requiresControlAuth)
               {
                  bool authPassed = m_authPassed;
                  if (repeatAuthEnabled)
                  {
                     authPassed = authPassed && m_authReqMessageId == messageId && m_repeatAuthPassed;
                  }
                  m_repeatAuthPassed = false;
                  if (!authPassed)
                  {
                     m_plogwriter->debug("Message requires control authentication");

                     m_pblockinggate->skipBytes(messageSize);
                     m_pblockinggate->writeUInt32(ControlProto::REPLY_AUTH_NEEDED);
                     m_authReqMessageId = messageId;

                     continue;
                  }
               }

               switch (messageId)
               {
                  case ControlProto::AUTH_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control authentication requested");
                     authMsgRcdv();
                     break;
                  case ControlProto::RELOAD_CONFIG_MSG_ID:
                     m_plogwriter->debug("Command requested: Reload configuration");
                     reloadConfigMsgRcvd();
                     break;
                  case ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID:
                     m_plogwriter->debug("Command requested: Disconnect all clients command requested");
                     disconnectAllMsgRcvd();
                     break;
                  case ControlProto::SHUTDOWN_SERVER_MSG_ID:
                     m_plogwriter->debug("Command requested: Shutdown command requested");
                     shutdownMsgRcvd();
                     break;
                  case ControlProto::ADD_CLIENT_MSG_ID:
                     m_plogwriter->debug("Command requested: Attach listening viewer");
                     addClientMsgRcvd();
                     break;
                  case ControlProto::CONNECT_TO_TCPDISP_MSG_ID:
                     m_plogwriter->debug("Connect to a tcp dispatcher command requested");
                     break;
                  case ControlProto::GET_SERVER_INFO_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests server info");
                     getServerInfoMsgRcvd();
                     break;
                  case ControlProto::GET_CLIENT_LIST_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests client ::list_base");
                     getClientsListMsgRcvd();
                     break;
                  case ControlProto::SET_CONFIG_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client sends new server config");
                     setServerConfigMsgRcvd();
                     break;
                  case ControlProto::GET_CONFIG_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests server config");
                     getServerConfigMsgRcvd();
                     break;
                  case ControlProto::GET_SHOW_TRAY_ICON_FLAG:
                     m_plogwriter->debug("::innate_subsystem::Control client requests tray icon visibility flag");
                     getShowTrayIconFlagMsgRcvd();
                     break;
                  case ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client sends process ID");
                     updateTvnControlProcessIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_PRIMARY_MSG_ID:
                     m_plogwriter->debug("Share primary scopedstrMessage recieved");
                     sharePrimaryIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_DISPLAY_MSG_ID:
                     m_plogwriter->debug("Share display scopedstrMessage recieved");
                     shareDisplayIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_WINDOW_MSG_ID:
                     m_plogwriter->debug("Share window scopedstrMessage recieved");
                     shareWindowIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_RECT_MSG_ID:
                     m_plogwriter->debug("Share rectangle scopedstrMessage recieved");
                     shareRectIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_FULL_MSG_ID:
                     m_plogwriter->debug("Share full scopedstrMessage recieved");
                     shareFullIdMsgRcvd();
                     break;
                  case ControlProto::SHARE_APP_MSG_ID:
                     m_plogwriter->debug("Share app scopedstrMessage recieved");
                     shareAppIdMsgRcvd();
                     break;
                  default:
                     m_pblockinggate->skipBytes(messageSize);
                     m_plogwriter->warning("Received unsupported scopedstrMessage from control client");
                     throw ControlException("Unknown command");
               } // switch (messageId).
            }
            catch (ControlException &controlEx)
            {
               m_plogwriter->error("::subsystem::Exception while processing control client's request: \"{}\"",
                                   controlEx.get_message());

               sendError(controlEx.get_message());
            }
         } // while
      }
      catch (::subsystem::Exception &ex)
      {
         m_plogwriter->error("::subsystem::Exception in control client thread: \"{}\"", ex.get_message());
      }
   }

   void ControlClient::onTerminate()
   {
      try
      {
         m_transport->close();
      }
      catch (...)
      {
      }
   }

   void ControlClient::sendError(const ::scoped_string &scopedstrMessage)
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_ERROR);
      m_pblockinggate->writeUTF8(scopedstrMessage);
   }

   //
   // FIXME: Code duplicate (see RfbInitializer class).
   //

   void ControlClient::authMsgRcdv()
   {
      unsigned char challenge[16];
      unsigned char response[16];

      srand((unsigned)time(0));
      for (int i = 0; i < sizeof(challenge); i++)
      {
         challenge[i] = rand() & 0xff;
      }

      m_pblockinggate->write(challenge, sizeof(challenge));
      m_pblockinggate->readFully(response, sizeof(response));

      //
      // FIXME: Is it right to check if password is set after client
      // sent password to us.
      //

      ::remoting::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();
      unsigned char cryptPassword[8];
      pserverconfig->getControlPassword(cryptPassword);

      bool isAuthSucceed = m_controlappauthenticator->authenticate(cryptPassword, challenge, response);
      if (!isAuthSucceed)
      {
         sendError(MainSubsystem().StringTable().getString(IDS_INVALID_CONTROL_PASSWORD));
      }
      else
      {
         m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);
         m_authPassed = true;
         m_repeatAuthPassed = true;
      }
   }

   void ControlClient::getClientsListMsgRcvd()
   {
      unsigned int clientCount = 0;

      RfbClientInfoList clients;

      m_rfbClientManager->getClientsInfo(&clients);

      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);
      _ASSERT(clients.size() == (unsigned int)clients.size());
      m_pblockinggate->writeUInt32((unsigned int)clients.size());

      for (RfbClientInfoList::iterator it = clients.begin(); it != clients.end(); it++)
      {
         m_pblockinggate->writeUInt32((*it).m_id);
         m_pblockinggate->writeUTF8((*it).m_peerAddr);
      }
   }

   void ControlClient::getServerInfoMsgRcvd()
   {
      bool acceptFlag = false;
      bool serviceFlag = false;

      ::string logPath;
      ::string statusText;

      ServerInfo info;


      ::cast<::remoting_node_desktop::application> papplication = ::system()->m_papplication;

      papplication->Server().getServerInfo(&info);

      ::string status;
      status = info.m_statusText;

      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      m_pblockinggate->writeUInt8(info.m_acceptFlag ? 1 : 0);
      m_pblockinggate->writeUInt8(info.m_serviceFlag ? 1 : 0);
      m_pblockinggate->writeUTF8(status);
   }

   void ControlClient::reloadConfigMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      m_pconfigurator->load();
   }

   void ControlClient::disconnectAllMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      m_rfbClientManager->disconnectAllClients();

      m_plogwriter->debug("Disconnecting clients");

      m_pthreadcollectorOutgoingConnection->destroyAllThreads();
   }

   void ControlClient::shutdownMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::cast<::remoting_node_desktop::application> papplication = ::system()->m_papplication;

      papplication->Server().generateExternalShutdownSignal();
   }

   void ControlClient::addClientMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      //
      // Read parameters.
      //

      ::string connectString;

      connectString = m_pblockinggate->readUtf8();

      bool viewOnly = m_pblockinggate->readUInt8() == 1;

      //
      // Parse host and port from connection string.
      //
      ::string connectStringAnsi(&connectString);
      ::remoting::HostPath hp(connectStringAnsi, 5500);

      if (!hp.isValid())
      {
         return;
      }

      //::string host;
      ::string host(hp.getVncHost());
      // sansiHost.toStringStorage(&host);

      //
      // Make outgoing connection in separate thread.
      //
      OutgoingRfbConnectionThread *newConnectionThread =
         new OutgoingRfbConnectionThread(host, hp.getVncPort(), viewOnly, m_rfbClientManager, m_plogwriter);

      newConnectionThread->resume();

      //  ZombieKiller::getInstance()->addZombie(newConnectionThread);
      m_pthreadcollectorOutgoingConnection->addThread(newConnectionThread);
   }


   bool allZeroes(unsigned char p[::remoting::ServerConfig::VNC_PASSWORD_SIZE])
   {
      for (int i = 0; i < ::remoting::ServerConfig::VNC_PASSWORD_SIZE; i++)
      {
         if (p[i] != 0)
         {
            return false;
         }
      }
      return true;
   }

   void ControlClient::setServerConfigMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);
      ::remoting::ServerConfig cfg;
      cfg.deserialize(m_pblockinggate);
      ::remoting::ServerConfig *old = m_pconfigurator->getServerConfig();
      unsigned char tmp[::remoting::ServerConfig::VNC_PASSWORD_SIZE];

      if (cfg.hasPrimaryPassword())
      {
         cfg.getPrimaryPassword(tmp);
         if (allZeroes(tmp))
         {
            // was not changed
            old->getPrimaryPassword(tmp);
            cfg.setPrimaryPassword(tmp);
         }
      }
      if (cfg.hasReadOnlyPassword())
      {
         cfg.getReadOnlyPassword(tmp);
         if (allZeroes(tmp))
         {
            // was not changed
            old->getReadOnlyPassword(tmp);
            cfg.setReadOnlyPassword(tmp);
         }
      }
      if (cfg.hasControlPassword())
      {
         cfg.getControlPassword(tmp);
         if (allZeroes(tmp))
         {
            // was not changed
            old->getControlPassword(tmp);
            cfg.setControlPassword(tmp);
         }
      }
      *old = cfg;
      m_pconfigurator->save();
      m_pconfigurator->load();
   }

   void ControlClient::getShowTrayIconFlagMsgRcvd()
   {
      bool showIcon = m_pconfigurator->getServerConfig()->getShowTrayIconFlag();

      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      m_pblockinggate->writeUInt8(showIcon ? 1 : 0);
   }

   void ControlClient::updateTvnControlProcessIdMsgRcvd()
   {
      m_pblockinggate->readUInt32();

      try
      {
         WindowsSubsystem().WTS().duplicatePipeClientToken(m_pfilePipeHandle);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Can't update the control client impersonation token: {}", e.get_message());
      }
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);
   }

   void ControlClient::getServerConfigMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ServerConfig cfg = *m_pconfigurator->getServerConfig();

      unsigned char zeroes[::remoting::ServerConfig::VNC_PASSWORD_SIZE] = {};
      if (cfg.hasControlPassword())
         cfg.setControlPassword(zeroes);
      if (cfg.hasPrimaryPassword())
         cfg.setPrimaryPassword(zeroes);
      if (cfg.hasReadOnlyPassword())
         cfg.setReadOnlyPassword(zeroes);

      cfg.serialize(m_pblockinggate);
   }

   void ControlClient::sharePrimaryIdMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);
      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setPrimaryDisplay();
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareDisplayIdMsgRcvd()
   {
      unsigned char displayNumber = m_pblockinggate->readUInt8();
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setDisplayNumber(displayNumber);
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareWindowIdMsgRcvd()
   {
      ::string windowName;
      windowName = m_pblockinggate->readUtf8();

      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setWindowName(windowName);
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareRectIdMsgRcvd()
   {
      ::int_rectangle shareRect;
      shareRect.left = m_pblockinggate->readInt32();
      shareRect.top = m_pblockinggate->readInt32();
      shareRect.right = m_pblockinggate->readInt32();
      shareRect.bottom = m_pblockinggate->readInt32();
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setArbitraryRect(shareRect);
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareFullIdMsgRcvd()
   {
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setFullDesktop();
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareAppIdMsgRcvd()
   {
      unsigned int procId = m_pblockinggate->readUInt32();
      m_pblockinggate->writeUInt32(ControlProto::REPLY_OK);

      ::remoting::ViewPortState dynViewPort;
      dynViewPort.setProcessId(procId);
      m_rfbClientManager->setDynViewPort(&dynViewPort);
   }


} // namespace remoting_node_desktop



