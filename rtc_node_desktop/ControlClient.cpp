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
#include "ControlClient.h"
#include "Server.h"
#include "OutgoingRfbConnectionThread.h"

#include "remoting/node_desktop/control_desktop/ControlProto.h"
#include "application.h"
//#include "remoting/remoting_rtc/network/socket/SocketStream.h"

#include "remoting/remoting_rtc/node_config/Configurator.h"
#include "remoting/remoting_rtc/node_config/ServerConfig.h"

#include "subsystem/platform/VncPassCrypt.h"
//#include "subsystem_windows/platform/subsystem.h"
//#include "subsystem_windows/platform/subsystem.h"
#include "subsystem/node/File.h"
#include "remoting/remoting_rtc/rfb/HostPath.h"

//#include "subsystem_windows/node/WTS.h"

#include "remoting/node_desktop/resource.h"
#include "subsystem/node/OperatingSystem.h"
//#include aaa_<time.h>
//#include "subsystem/platform/::string.h"
#include "subsystem/platform/MemUsage.h"

namespace remoting_node_desktop
{


   const ::u32 ControlClient::REQUIRES_AUTH[] = {
      ::remoting_control_desktop::ControlProto::ADD_CLIENT_MSG_ID,      ::remoting_control_desktop::ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID,
      ::remoting_control_desktop::ControlProto::GET_CONFIG_MSG_ID,      ::remoting_control_desktop::ControlProto::SET_CONFIG_MSG_ID,
      ::remoting_control_desktop::ControlProto::SHUTDOWN_SERVER_MSG_ID, ::remoting_control_desktop::ControlProto::SHARE_PRIMARY_MSG_ID,
      ::remoting_control_desktop::ControlProto::SHARE_DISPLAY_MSG_ID,   ::remoting_control_desktop::ControlProto::SHARE_WINDOW_MSG_ID,
      ::remoting_control_desktop::ControlProto::SHARE_RECT_MSG_ID,      ::remoting_control_desktop::ControlProto::SHARE_APP_MSG_ID,
      ::remoting_control_desktop::ControlProto::SHARE_FULL_MSG_ID,      ::remoting_control_desktop::ControlProto::CONNECT_TO_TCPDISP_MSG_ID};

   const ::u32 ControlClient::WITHOUT_AUTH[] = {::remoting_control_desktop::ControlProto::AUTH_MSG_ID,
                                                       ::remoting_control_desktop::ControlProto::RELOAD_CONFIG_MSG_ID,
                                                       ::remoting_control_desktop::ControlProto::GET_SERVER_INFO_MSG_ID,
                                                       ::remoting_control_desktop::ControlProto::GET_CLIENT_LIST_MSG_ID,
                                                       ::remoting_control_desktop::ControlProto::GET_SHOW_TRAY_ICON_FLAG,
                                                       ::remoting_control_desktop::ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID};

   ControlClient::ControlClient(::remoting_rtc_node::Configurator * pconfigurator, ::remoting_control_desktop::Transport *transport, RfbClientManager *rfbClientManager,
                                ControlAppAuthenticator *pauthenticator, ::subsystem::FileInterface *pfilePipeHandle,
                                ::subsystem::LogWriter * plogwriter) :
       m_pconfigurator(pconfigurator), m_ptransport(transport), m_prfbclientmanager(rfbClientManager), m_pcontrolappauthenticator(pauthenticator), m_uTcpDispId(0),
       m_pfilePipeHandle(pfilePipeHandle), m_authReqMessageId(0), m_plogwriter(plogwriter), m_repeatAuthPassed(false)
   {
      m_pchannel = m_ptransport->getIOStream();

      m_pcontrolgate = new ::remoting_control_desktop::ControlGate(m_pchannel);

      m_authPassed = false;
   }

   ControlClient::~ControlClient()
   {
      //setThreadToFinish();
      //waitThreadToFinish();

      //delete m_pcontrolgate;
      //delete m_ptransport;
   }

void ControlClient::destroy()
{
   
   ::subsystem::Thread::destroy();
   
   //setThreadToFinish();
   //waitThreadToFinish();
   
}

   void ControlClient::onThreadMain()
   {
      // Client passes authentication by default if server does not uses control authentication.
      if (!m_pconfigurator->getServerConfig()->isControlAuthEnabled())
      {
         m_authPassed = true;
      }
      int i = 0;
      try
      {
         while (!isThreadTerminating())
         {
            ::u32 messageId = m_pcontrolgate->readUInt32();
            ::u32 messageSize = m_pcontrolgate->readUInt32();

            m_plogwriter->debug("Recieved control scopedstrMessage ID %u, size %u", (::u32)messageId,
                                (::u32)messageSize);
            if (++i % 10 == 0)
            {

               m_plogwriter->debug("process memory usage: {} ", MainSubsystem().getCurrentMemoryUsage());
            }
            bool requiresControlAuth = m_pconfigurator->getServerConfig()->isControlAuthEnabled();
            bool repeatAuthEnabled = m_pconfigurator->getServerConfig()->getControlAuthAlwaysChecking();

            // Check if scopedstrMessage requires Remoting admin privilegies.
            if (requiresControlAuth)
            {
               for (size_t i = 0; i < sizeof(WITHOUT_AUTH) / sizeof(::u32); i++)
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

                     m_pcontrolgate->skipBytes(messageSize);
                     m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_AUTH_NEEDED);
                     m_authReqMessageId = messageId;

                     continue;
                  }
               }

               switch (messageId)
               {
                  case ::remoting_control_desktop::ControlProto::AUTH_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control authentication requested");
                     authMsgRcdv();
                     break;
                  case ::remoting_control_desktop::ControlProto::RELOAD_CONFIG_MSG_ID:
                     m_plogwriter->debug("Command requested: Reload configuration");
                     reloadConfigMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID:
                     m_plogwriter->debug("Command requested: Disconnect all clients command requested");
                     disconnectAllMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHUTDOWN_SERVER_MSG_ID:
                     m_plogwriter->debug("Command requested: Shutdown command requested");
                     shutdownMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::ADD_CLIENT_MSG_ID:
                     m_plogwriter->debug("Command requested: Attach listening viewer");
                     addClientMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::CONNECT_TO_TCPDISP_MSG_ID:
                     m_plogwriter->debug("Connect to a tcp dispatcher command requested");
                     break;
                  case ::remoting_control_desktop::ControlProto::GET_SERVER_INFO_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests server info");
                     getServerInfoMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::GET_CLIENT_LIST_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests client ::list_base");
                     getClientsListMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SET_CONFIG_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client sends new server config");
                     setServerConfigMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::GET_CONFIG_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client requests server config");
                     getServerConfigMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::GET_SHOW_TRAY_ICON_FLAG:
                     m_plogwriter->debug("::innate_subsystem::Control client requests tray icon visibility flag");
                     getShowTrayIconFlagMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID:
                     m_plogwriter->debug("::innate_subsystem::Control client sends process ID");
                     updateTvnControlProcessIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_PRIMARY_MSG_ID:
                     m_plogwriter->debug("Share primary scopedstrMessage recieved");
                     sharePrimaryIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_DISPLAY_MSG_ID:
                     m_plogwriter->debug("Share display scopedstrMessage recieved");
                     shareDisplayIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_WINDOW_MSG_ID:
                     m_plogwriter->debug("Share window scopedstrMessage recieved");
                     shareWindowIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_RECT_MSG_ID:
                     m_plogwriter->debug("Share rectangle scopedstrMessage recieved");
                     shareRectIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_FULL_MSG_ID:
                     m_plogwriter->debug("Share full scopedstrMessage recieved");
                     shareFullIdMsgRcvd();
                     break;
                  case ::remoting_control_desktop::ControlProto::SHARE_APP_MSG_ID:
                     m_plogwriter->debug("Share app scopedstrMessage recieved");
                     shareAppIdMsgRcvd();
                     break;
                  default:
                     m_pcontrolgate->skipBytes(messageSize);
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

   void ControlClient::onTermThread()
   {
      try
      {
         m_ptransport->close();
      }
      catch (...)
      {
      }
   }

   void ControlClient::sendError(const ::scoped_string &scopedstrMessage)
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_ERROR);
      m_pcontrolgate->writeUTF8(scopedstrMessage);
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

      m_pcontrolgate->write(challenge, sizeof(challenge));
      m_pcontrolgate->readFully(response, sizeof(response));

      //
      // FIXME: Is it right to check if password is set after client
      // sent password to us.
      //

      ::remoting_rtc_node::ServerConfig * pserverconfig = m_pconfigurator->getServerConfig();
      unsigned char cryptPassword[8];
      pserverconfig->getControlPassword(cryptPassword);

      bool isAuthSucceed = m_pcontrolappauthenticator->authenticate(cryptPassword, challenge, response);
      if (!isAuthSucceed)
      {
         sendError(MainSubsystem().StringTable().getString(IDS_INVALID_CONTROL_PASSWORD));
      }
      else
      {
         m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);
         m_authPassed = true;
         m_repeatAuthPassed = true;
      }
   }

   void ControlClient::getClientsListMsgRcvd()
   {
      ::u32 clientCount = 0;

      ::remoting_control_desktop::RfbClientInfoList clients;

      m_prfbclientmanager->getClientsInfo(&clients);

      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);
      ASSERT(clients.size() == (::u32)clients.size());
      m_pcontrolgate->writeUInt32((::u32)clients.size());

      for (auto it = clients.begin(); it != clients.end(); it++)
      {
         m_pcontrolgate->writeUInt32((*it).m_id);
         m_pcontrolgate->writeUTF8((*it).m_peerAddr);
      }
   }

   void ControlClient::getServerInfoMsgRcvd()
   {
      bool acceptFlag = false;
      bool serviceFlag = false;

      ::string logPath;
      ::string statusText;

      ::remoting_control_desktop::ServerInfo info;


      ::cast<::remoting_node_desktop::application> papplication = ::system()->m_papplication;

      papplication->Server().getServerInfo(&info);

      ::string status;
      status = info.m_statusText;

      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      m_pcontrolgate->writeUInt8(info.m_acceptFlag ? 1 : 0);
      m_pcontrolgate->writeUInt8(info.m_serviceFlag ? 1 : 0);
      m_pcontrolgate->writeUTF8(status);
   }

   void ControlClient::reloadConfigMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      m_pconfigurator->load();
   }

   void ControlClient::disconnectAllMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      m_prfbclientmanager->disconnectAllClients();

      m_plogwriter->debug("Disconnecting clients");

      m_pthreadcollectorOutgoingConnection->destroyAllThreads();
   }

   void ControlClient::shutdownMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::cast<::remoting_node_desktop::application> papplication = ::system()->m_papplication;

      papplication->Server().generateExternalShutdownSignal();
   }

   void ControlClient::addClientMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      //
      // Read parameters.
      //

      ::string connectString;

      connectString = m_pcontrolgate->readUtf8();

      bool viewOnly = m_pcontrolgate->readUInt8() == 1;

      //
      // Parse host and port from connection string.
      //
      ::string connectStringAnsi(&connectString);
      ::remoting_rtc::HostPath hp(connectStringAnsi, 5500);

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
         new OutgoingRfbConnectionThread(host, hp.getVncPort(), viewOnly, m_prfbclientmanager, m_plogwriter);

      newConnectionThread->resumeThread();

      //  ZombieKiller::getInstance()->addZombie(newConnectionThread);
      m_pthreadcollectorOutgoingConnection->addThread(newConnectionThread);
   }


   bool allZeroes(unsigned char p[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE])
   {
      for (int i = 0; i < ::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE; i++)
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
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);
      ::remoting_rtc_node::ServerConfig cfg;
      cfg.deserialize(m_pcontrolgate);
      ::remoting_rtc_node::ServerConfig *old = m_pconfigurator->getServerConfig();
      unsigned char tmp[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE];

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

      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      m_pcontrolgate->writeUInt8(showIcon ? 1 : 0);
   }

   void ControlClient::updateTvnControlProcessIdMsgRcvd()
   {
      m_pcontrolgate->readUInt32();

      try
      {
         //WindowsSubsystem().WTS().duplicatePipeClientToken(m_pfilePipeHandle);
         MainSubsystem().OperatingSystem().duplicatePipeClientToken(m_pfilePipeHandle);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Can't update the control client impersonation token: {}", e.get_message());
      }
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);
   }

   void ControlClient::getServerConfigMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc_node::ServerConfig cfg = *m_pconfigurator->getServerConfig();

      unsigned char zeroes[::subsystem::VncPassCrypt::VNC_PASSWORD_SIZE] = {};
      if (cfg.hasControlPassword())
         cfg.setControlPassword(zeroes);
      if (cfg.hasPrimaryPassword())
         cfg.setPrimaryPassword(zeroes);
      if (cfg.hasReadOnlyPassword())
         cfg.setReadOnlyPassword(zeroes);

      cfg.serialize(m_pcontrolgate);
   }

   void ControlClient::sharePrimaryIdMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);
      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setPrimaryDisplay();
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareDisplayIdMsgRcvd()
   {
      unsigned char displayNumber = m_pcontrolgate->readUInt8();
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setDisplayNumber(displayNumber);
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareWindowIdMsgRcvd()
   {
      ::string windowName;
      windowName = m_pcontrolgate->readUtf8();

      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setWindowName(windowName);
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareRectIdMsgRcvd()
   {
      ::i32_rectangle shareRect;
      shareRect.left = m_pcontrolgate->readInt32();
      shareRect.top = m_pcontrolgate->readInt32();
      shareRect.right = m_pcontrolgate->readInt32();
      shareRect.bottom = m_pcontrolgate->readInt32();
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setArbitraryRect(shareRect);
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareFullIdMsgRcvd()
   {
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setFullDesktop();
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }

   void ControlClient::shareAppIdMsgRcvd()
   {
      ::u32 procId = m_pcontrolgate->readUInt32();
      m_pcontrolgate->writeUInt32(::remoting_control_desktop::ControlProto::REPLY_OK);

      ::remoting_rtc::ViewPortState dynViewPort;
      dynViewPort.setProcessId(procId);
      m_prfbclientmanager->setDynViewPort(&dynViewPort);
   }


} // namespace remoting_node_desktop



