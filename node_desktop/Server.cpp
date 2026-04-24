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
#include "Server.h"
#include "WsConfigRunner.h"
#include "AdditionalActionApplication.h"
#include "subsystem/node/CurrentConsoleProcess.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/security/SecurityAttributes.h"
#include "remoting/node_desktop/server_config/Configurator.h"

#include "subsystem/thread/GlobalMutex.h"

#include "remoting/node_desktop/resource.h"

//#include "remoting/remoting/wsconfig/TvnLogFilename.h"

///#include "remoting/remoting/network/socket/WindowsSocket.h"

#include "subsystem/platform/StringTable.h"
//#include "subsystem/platform/::string.h"
#include "remoting/node_desktop/NamingDefs.h"

#include "subsystem/node/file.h"
#include "subsystem/thread/ZombieKiller.h"
// FIXME: Bad dependency on remoting_control_desktop.
#include "remoting/control_desktop/TransportFactory.h"
#include "remoting/control_desktop/ControlPipeName.h"

#include "remoting/node_desktop/BuildTime.h"

//#include aaa_<crtdbg.h>
//#include aaa_<time.h>
#include "TimeAPI.h"


namespace remoting_node_desktop
{


   //Server::Server(bool runsInServiceContext,
   //                     NewConnectionEvents *newConnectionEvents,
   //                     LogInitListener *logInitListener,
   //                     ::subsystem::LogWriter * plogwriter)
   ////: Singleton<Server>(),
   //:  ListenerContainer<ServerListener *>(),
   //  m_runAsService(runsInServiceContext),
   //  m_ploginitlistener(logInitListener),
   //  m_prfbclientmanager(0),
   //  //m_httpServer(0),
   //   m_pcontrolserver(0), m_prfbserver(0),
   //  m_config(runsInServiceContext),
   //  //m_plogwriter(::subsystem::LogWriter),
   //    m_plogwriter(plogwriter),
   //  m_contextSwitchResolution(1),
   //  //m_pextrarfbservers(&m_plogwriter)
   //    m_pextrarfbservers(::system())
   //{
   //   m_plogwriter->information("{} Build on {}",
   //                  ProductNames::SERVER_PRODUCT_NAME,
   //                  BuildTime::DATE);

   //   // Initialize configuration.
   //   // FIXME: It looks like configurator may be created as a member object.
   //   Configurator *configurator = Configurator::getInstance();
   //   configurator->load();
   //   m_pserverconfig = Configurator::getInstance()->getServerConfig();

   //   try {
   //      ::string logDir;
   //      m_pserverconfig->getLogFileDir(logDir);
   //      unsigned char logLevel = m_pserverconfig->getLogLevel();
   //      // FIXME: Use correct log name.
   //      m_ploginitlistener->onLogInit(logDir, LogNames::SERVER_LOG_FILE_STUB_NAME, logLevel);

   //   } catch (...) {
   //      // A log error must not be a reason that stop the server.
   //   }

   //   // Initialize windows sockets.

   //   m_plogwriter->information("Initialize WinSock");


   //   MainSubsystem().startSockets();
   //   // try {
   //   //   WindowsSocket::startup(2, 1);
   //   // } catch (::subsystem::Exception &ex) {
   //   //   m_plogwriter->interror("{}", ex.get_message());
   //   // }

   //   DesktopFactory *desktopFactory = 0;
   //   if (runsInServiceContext) {
   //      desktopFactory = &m_servicedesktopfactory;
   //   } else {
   //      desktopFactory = &m_applicationdesktopfactory;
   //   }

   //   m_prfbclientmanager = new RfbClientManager(0, newConnectionEvents, m_plogwriter, desktopFactory);

   //   m_prfbclientmanager->addListener(this);

   //   // FIXME: No good to act as a listener before completing the object
   //   //        construction.
   //   Configurator::getInstance()->addListener(this);

   //   {
   //      // FIXME: Protect only primitive operations.
   //      // FIXME: Nested lock in protected code (congifuration locking).
   //      critical_section_lock l(&m_mutex);

   //      restartMainRfbServer();
   //      (void)m_pextrarfbservers->reload(m_runAsService, m_prfbclientmanager);
   //      restartHttpServer();
   //      restartControlServer();
   //   }
   //}

   Server::Server() : m_bRunAsService(false)
   {


   }

   Server::~Server()
   {
      Configurator::getInstance()->removeListener(this);

      stopControlServer();
      stopHttpServer();
      m_pextrarfbservers->shutDown();
      stopMainRfbServer();

      //auto ZombieKiller *zombieKiller = ZombieKiller::getInstance();

      // Disconnect all zombies http, rfb, control clients though killing
      // their threads.
      MainSubsystem().ZombieKiller().killAllZombies();

      m_prfbclientmanager->removeListener(this);

      delete m_prfbclientmanager;

      m_plogwriter->information("Shutdown WinSock");

      // try {
      //    WindowsSocket::cleanup();
      // } catch (::subsystem::Exception &ex) {
      //    m_plogwriter->error("{}", ex.get_message());
      // }
      MainSubsystem().cleanupSockets();
   }


   void Server::initialize_remoting_node_desktop_server(bool runsInServiceContext, NewConnectionEvents *newConnectionEvents, LogInitListener *logInitListener,
                  ::subsystem::LogWriter *plogwriter)
   {
      //: Singleton<Server>(),
       //:
      //ListenerContainer<ServerListener *>();
      m_bRunAsService = runsInServiceContext;
      m_ploginitlistener = logInitListener;
      m_prfbclientmanager = nullptr;
      // m_httpServer(0),
      m_pcontrolserver = nullptr;
      m_prfbserver = nullptr;
      //m_config = runsInServiceContext;
      construct_newø(m_pconfigurator);
      m_pconfigurator->initialize_configurator(m_bRunAsService);
       // m_plogwriter(::subsystem::LogWriter),
      m_plogwriter = plogwriter;
      m_contextSwitchResolution = TRUE;
      // m_pextrarfbservers(&m_plogwriter)
      construct_newø(m_pextrarfbservers);
      m_pextrarfbservers->initialize_extra_rfb_servers(m_pconfigurator, m_plogwriter);
      m_plogwriter->information("{} Build on {}", ProductNames::SERVER_PRODUCT_NAME, BuildTime::DATE);

      // Initialize configuration.
      // FIXME: It looks like configurator may be created as a member object.
      Configurator *configurator = Configurator::getInstance();
      configurator->load();
      m_pserverconfig = Configurator::getInstance()->getServerConfig();

      try
      {
         ::string logDir;
         m_pserverconfig->getLogFileDir(logDir);
         unsigned char logLevel = m_pserverconfig->getLogLevel();
         // FIXME: Use correct log name.
         m_ploginitlistener->onLogInit(logDir, LogNames::SERVER_LOG_FILE_STUB_NAME, logLevel);
      }
      catch (...)
      {
         // A log error must not be a reason that stop the server.
      }

      // Initialize windows sockets.

      m_plogwriter->information("Initialize WinSock");


      MainSubsystem().startSockets();
      // try {
      //   WindowsSocket::startup(2, 1);
      // } catch (::subsystem::Exception &ex) {
      //   m_plogwriter->interror("{}", ex.get_message());
      // }

      DesktopFactory *desktopFactory = 0;
      if (runsInServiceContext)
      {
         desktopFactory = &m_servicedesktopfactory;
      }
      else
      {
         desktopFactory = &m_applicationdesktopfactory;
      }

      m_prfbclientmanager = new RfbClientManager(0, newConnectionEvents, m_plogwriter, desktopFactory);

      m_prfbclientmanager->addListener(this);

      // FIXME: No good to act as a listener before completing the object
      //        construction.
      Configurator::getInstance()->addListener(this);

      {
         // FIXME: Protect only primitive operations.
         // FIXME: Nested lock in protected code (congifuration locking).
         critical_section_lock l(&m_mutex);

         restartMainRfbServer();
         (void)m_pextrarfbservers->reload(m_runAsService, m_prfbclientmanager);
         restartHttpServer();
         restartControlServer();
      }
   }

   // Remark: this method can be called from other threads.
   void Server::onConfigReload(ServerConfig *serverConfig)
   {
      // Start/stop/restart RFB servers if needed.
      {
         // FIXME: Protect only primitive operations.
         // FIXME: Nested lock in protected code (congifuration locking).
         critical_section_lock l(&m_mutex);

         bool toggleMainRfbServer =
           m_pserverconfig->isAcceptingRfbConnections() != (m_prfbserver != 0);
         bool changeMainRfbPort = m_prfbserver != 0 &&
           (m_pserverconfig->getRfbPort() != (int)m_prfbserver->getBindPort());

         ::string strBindHost =
           m_pserverconfig->isOnlyLoopbackConnectionsAllowed() ? "localhost" : "0.0.0.0";
         bool changeBindHost =  m_prfbserver != 0 &&
          strBindHost != m_prfbserver->getBindHost();

         if (toggleMainRfbServer ||
             changeMainRfbPort ||
             changeBindHost) {
           restartMainRfbServer();
         }

         // NOTE: ExtraRfbServers::reload() does not throw exceptions if some
         //       servers did not start. However, it returns false in that case.
         //       Here we ignore all errors.
         (void)m_pextrarfbservers->reload(m_runAsService, m_prfbclientmanager);
       }

      // // Start/stop/restart HTTP server if needed.
      // {
      //    // FIXME: Protect only primitive operations.
      //    critical_section_lock l(&m_mutex);
      //
      //    bool toggleHttp =
      //      m_pserverconfig->isAcceptingHttpConnections() != (m_httpServer != 0);
      //    bool changePort = m_httpServer != 0 &&
      //      (m_pserverconfig->getHttpPort() != (int)m_httpServer->getBindPort());
      //
      //    if (toggleHttp || changePort) {
      //       restartHttpServer();
      //    }
      // }
      changeLogProps();

   }


   void Server::getServerInfo(ServerInfo *info)
   {
      bool rfbServerListening = true;
      {
         critical_section_lock l(&m_mutex);
         rfbServerListening = m_prfbserver != 0;
      }

      ::string statusString;

      // Vnc authentication enabled.
      bool vncAuthEnabled = m_pserverconfig->isUsingAuthentication();
      // No vnc passwords are set.
      bool noVncPasswords = !m_pserverconfig->hasPrimaryPassword() && !m_pserverconfig->hasReadOnlyPassword();
      // Determinates that main rfb server cannot accept connection in case of passwords problem.
      bool vncPasswordsError = vncAuthEnabled && noVncPasswords;

      if (rfbServerListening) {
         if (vncPasswordsError) {
            statusString = MainSubsystem().StringTable().getString(IDS_NO_PASSWORDS_SET);
         } else {
            // FIXME: Usage of deprecated FUNCTION!
            ::string localAddressString = MainSubsystem().getLocalIpAddressString();
            statusString = localAddressString;

            if (!vncAuthEnabled) {
               statusString += MainSubsystem().StringTable().getString(IDS_NO_AUTH_STATUS);
            } // if no auth enabled.
         } // accepting connections and no problem with passwords.
      } else {
         statusString = MainSubsystem().StringTable().getString(IDS_SERVER_NOT_LISTENING);
      } // not accepting connections.

      unsigned int stringId = m_runAsService ? IDS_TVNSERVER_SERVICE : IDS_TVNSERVER_APP;

      info->m_statusText.format("{} - {}",
                                MainSubsystem().StringTable().getString(stringId),
                                statusString);
      info->m_acceptFlag = rfbServerListening && !vncPasswordsError;
      info->m_serviceFlag = m_runAsService;
   }

   void Server::generateExternalShutdownSignal()
   {
      critical_section_lock l(&m_listeners);

      //::std::vector<ServerListener *>::iterator it;
      for (auto it = m_listeners.begin(); it != m_listeners.end(); it++) {
         ServerListener *each = *it;

         each->onServerShutdown();
      } // for all listeners.
   }

   bool Server::isRunningAsService() const
   {
      return m_runAsService;
   }

   void Server::afterFirstClientConnect()
   {
      if (timeBeginPeriod(m_contextSwitchResolution) == TIMERR_NOERROR) {
         m_plogwriter->debug("Set context switch resolution: {} ms", m_contextSwitchResolution);
      }
      else {
         m_plogwriter->debug("Can't change context switch resolution to: {} ms", m_contextSwitchResolution);
      }

   }

   void Server::afterLastClientDisconnect()
   {
      m_plogwriter->debug("Restore context switch resolution");
      timeEndPeriod(m_contextSwitchResolution);

      ServerConfig::DisconnectAction action = m_pserverconfig->getDisconnectAction();

      // Disconnect action must be executed in process on interactive user session to take effect.
      // Now, choose application keys for specified action.

      ::string keys;

      switch (action) {
         case ServerConfig::DA_LOCK_WORKSTATION:
            keys.formatf("{}", AdditionalActionApplication::LOCK_WORKSTATION_KEY);
            break;
         case ServerConfig::DA_LOGOUT_WORKSTATION:
            keys.formatf("{}", AdditionalActionApplication::LOGOUT_KEY);
            break;
         default:
            return;
      }

      ::pointer < ::subsystem::ProcessInterface > pprocess;

      // Choose how to start process.
      ::string thisModulePath;
      thisModulePath = MainSubsystem().OperatingSystem().getCurrentModuleFolderPath();
      thisModulePath.double_quote();
      if (isRunningAsService()) {
         bool connectToRdp = m_pserverconfig->getConnectToRdpFlag();
         auto pprocessNew = createø <subsystem::CurrentConsoleProcess>();
         pprocessNew->initialize_current_console_process(m_plogwriter, connectToRdp, thisModulePath,
                                             keys);
         pprocess = pprocessNew;
      } else {

         auto pprocessNew = createø< subsystem::Process >();
         pprocessNew->initialize_process(thisModulePath, keys);
         pprocess = pprocessNew;
      }

      m_plogwriter->information("Execute disconnect action in separate process");

      try {
         pprocess->start();
      } catch (::subsystem::SystemException &ex) {
         m_plogwriter->error("Failed to start application: \"{}\"", ex.get_message());
      }

      //delete process;
   }

   void Server::restartHttpServer()
   {
      // FIXME: Errors are critical here, they should not be ignored.

      stopHttpServer();

      // if (m_pserverconfig->isAcceptingHttpConnections()) {
      //    m_plogwriter->debug("Starting HTTP server");
      //    try {
      //       // FIXME: HTTP server should bind to localhost if only loopback
      //       //        connections are allowed.
      //       m_httpServer = new HttpServer("0.0.0.0", m_pserverconfig->getHttpPort(), m_runAsService, &m_plogwriter);
      //    } catch (::subsystem::Exception &ex) {
      //       m_plogwriter->error("Failed to start HTTP server: \"{}\"", ex.get_message());
      //    }
      // }
   }

   void Server::restartControlServer()
   {
      // FIXME: Memory leaks.
      // FIXME: Errors are critical here, they should not be ignored.

      stopControlServer();

      m_plogwriter->debug("Starting control server");

      try {
         ::string pipeName;
         ControlPipeName::createPipeName(isRunningAsService(), pipeName, m_plogwriter);

         // FIXME: Memory leak
         auto psecurityattributes = createø<::subsystem::SecurityAttributes >();
         psecurityattributes->setInheritable();
         psecurityattributes->shareToAllUsers();

         const unsigned int maxControlServerPipeBufferSize = 0x10000;
         auto ppipeserver = createø< ::subsystem::PipeServer>();
         ppipeserver->initialize_pipe_server(pipeName, maxControlServerPipeBufferSize, psecurityattributes);
         m_pcontrolserver = new ControlServer(ppipeserver , m_prfbclientmanager, m_plogwriter);
      } catch (::subsystem::Exception &ex) {
         m_plogwriter->error("Failed to start control server: \"{}\"", ex.get_message());
      }
   }

   void Server::restartMainRfbServer()
   {
      // FIXME: Errors are critical here, they should not be ignored.

      stopMainRfbServer();

      if (!m_pserverconfig->isAcceptingRfbConnections()) {
         return;
      }

      ::string strBindHost = m_pserverconfig->isOnlyLoopbackConnectionsAllowed() ? "localhost" : "0.0.0.0";
      unsigned short bindPort = m_pserverconfig->getRfbPort();

      m_plogwriter->debug("Starting main RFB server");

      try {
        m_prfbserver = new RfbServer(strBindHost, bindPort, m_prfbclientmanager, m_runAsService, m_plogwriter);
      } catch (::subsystem::Exception &ex) {
        m_plogwriter->error("Failed to start main RFB server: \"{}\"", ex.get_message());
      }
    }

   void Server::stopHttpServer()
   {
      m_plogwriter->debug("Stopping HTTP server");

      // HttpServer *httpServer = 0;
      // {
      //    critical_section_lock l(&m_mutex);
      //    httpServer = m_httpServer;
      //    m_httpServer = 0;
      // }
      // if (httpServer != 0) {
      //    delete httpServer;
      // }
   }

   void Server::stopControlServer()
   {
      m_plogwriter->debug("Stopping control server");

      ControlServer *controlServer = 0;
      {
         critical_section_lock l(&m_mutex);
         controlServer = m_pcontrolserver;
         m_pcontrolserver = 0;
      }
      if (controlServer != 0) {
         delete controlServer;
      }
   }

   void Server::stopMainRfbServer()
   {
      m_plogwriter->debug("Stopping main RFB server");

      RfbServer *rfbServer = 0;
      {
         critical_section_lock l(&m_mutex);
         rfbServer = m_prfbserver;
         m_prfbserver = 0;
      }
      if (rfbServer != 0) {
         delete rfbServer;
      }
   }

   void Server::changeLogProps()
   {
      ::string logDir;
      unsigned char logLevel;
      {
         critical_section_lock al(&m_mutex);
         m_pserverconfig->getLogFileDir(logDir);
         logLevel = m_pserverconfig->getLogLevel();
      }
      m_ploginitlistener->onChangeLogProps(logDir, logLevel);
   }
} // namespace remoting_node_desktop