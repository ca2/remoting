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
#include "DesktopServerApplication.h"
#include "DesktopServerCommandLine.h"
#include "subsystem/platform/ResourceLoader.h"
#include "remoting/remoting_rfb/desktop/WallpaperUtil.h"
#include "subsystem/node/WTS.h"
#include "subsystem/node/OperatingSystem.h"
//#include "subsystem/node/SharedMemory.h"
#include "remoting/remoting_rfb/node/NamingDefs.h"
#include "subsystem/platform/subsystem.h"
//#include "subsystem_windows/node/WTS.h"
//#include "subsystem_windows/platform/subsystem.h"
//#include "subsystem_windows/platform/subsystem.h"
//#include "subsystem_windows/node/SharedMemory.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#ifdef WINDOWS
#include "TimeAPI.h"
#endif

namespace remoting_rfb_node_desktop
{

   DesktopServerApplication::DesktopServerApplication(
#ifdef WINDOWS
      ::hinstance appInstance, const ::scoped_string &scopedstrwindowClassName,
#endif
      const ::subsystem::CommandLineArguments *cmdArgs) : // LocalWindowsApplication(appInstance, windowClassName),
       m_pchannelClientToServer(0), m_pchannelServerToClient(0), m_pgateClientToServer(0), m_pgateServerToClient(0), m_pdesktopsrvdispatcher(0), m_pupdatehandlerserver(0),
       m_puserinputserver(0), m_pconfigserver(0), m_pgatekickhandler(0), m_psessionchangeswatcher(0),// m_configurator(true),
       // m_clientLogWriter(LogNames::LOG_PIPE_PUBLIC_NAME,
       //             LogNames::SERVER_LOG_FILE_STUB_NAME),
       m_contextSwitchResolution(1),
       // m_plogwriter(&m_clientLogWriter)
       m_plogwriter(::system())
   {
      initialize_operating_system_application();
      // try {
      //   m_clientLogWriter.connect();
      // } catch (...) {
      // }

      DesktopServerCommandLine cmdLineParser;

      cmdLineParser.parse(cmdArgs);

      // Keep session id
      //DWORD baseSessionId = WindowsSubsystem().WTS().getActiveConsoleSessionId(m_plogwriter);
      auto baseSessionId = MainSubsystem().OperatingSystem().getActiveConsoleSessionId(m_plogwriter);
      m_pconfigurator->addListener(this);
      m_pconfigurator->load();

      try
      {
         // Transport initialization
         // Get pipe channel handles by the shared memory
         ::string shMemName;
         cmdLineParser.getSharedMemName(shMemName);

         auto memory = MainSubsystem().OperatingSystem().getSharedMemorySnapshot(shMemName, 72, 10_s);

         auto mem= (::u64 *) memory.data();

#ifdef WINDOWS
         HANDLE readPipeHandle, writePipeHandle;
#else
         int readPipeHandle, writePipeHandle;
#endif
         ::u32 maxPortionSize;

#ifdef WINDOWS
         readPipeHandle = (HANDLE)mem[1];
         writePipeHandle = (HANDLE)mem[2];
#else
         readPipeHandle = (int)mem[1];
         writePipeHandle = (int)mem[2];
#endif
         maxPortionSize = (::u32)mem[3];
         auto ppipeClient = create_newø<::subsystem::AnonymousPipe>();
#ifdef WINDOWS
         auto pfileClientRead = MainSubsystem().fileFrom_HANDLE(readPipeHandle);
         auto pfileClientWrite = MainSubsystem().fileFrom_HANDLE(writePipeHandle);
#else
         auto pfileClientRead = MainSubsystem().fileFrom_fd(readPipeHandle);
         auto pfileClientWrite = MainSubsystem().fileFrom_fd(writePipeHandle);

#endif
         ppipeClient->initialize_anonymous_pipe(pfileClientRead, pfileClientWrite, maxPortionSize, m_plogwriter);
         // m_pchannelClientToServer = new AnonymousPipe(readPipeHandle, writePipeHandle, maxPortionSize, &m_plogwriter);
         // m_pchannelClientToServer = new AnonymousPipe(readPipeHandle, writePipeHandle, maxPortionSize, &m_plogwriter);
         m_pchannelClientToServer = ppipeClient;
         m_plogwriter->informationf("Client->server readPipeHandle = %p, writePipeHandle = %p", readPipeHandle,
                                    writePipeHandle);

#ifdef WINDOWS

         readPipeHandle = (HANDLE)mem[4];
         writePipeHandle = (HANDLE)mem[5];
#else

         readPipeHandle = (int)mem[4];
         writePipeHandle = (int)mem[5];

#endif
         maxPortionSize = (::u32)mem[6];
         auto ppipeServer = create_newø<::subsystem::AnonymousPipe>();
#ifdef WINDOWS

         auto pfileServerRead = MainSubsystem().fileFrom_HANDLE(readPipeHandle);
         auto pfileServerWrite = MainSubsystem().fileFrom_HANDLE(writePipeHandle);
#else
         auto pfileServerRead = MainSubsystem().fileFrom_fd(readPipeHandle);
         auto pfileServerWrite = MainSubsystem().fileFrom_fd(writePipeHandle);

#endif
         ppipeServer->initialize_anonymous_pipe(pfileServerRead, pfileServerWrite, maxPortionSize, m_plogwriter);
         // m_pchannelServerToClient = new AnonymousPipe(readPipeHandle, writePipeHandle, maxPortionSize, &m_plogwriter);
         m_pchannelServerToClient = ppipeServer;
         m_plogwriter->informationf("Server->client readPipeHandle = %p, writePipeHandle = %p", readPipeHandle,
                                    writePipeHandle);

         m_pgateClientToServer = new ::remoting_rfb::BlockingGate(m_pchannelClientToServer);
         m_pgateServerToClient = new ::remoting_rfb::BlockingGate(m_pchannelServerToClient);

         // Server initializations
         m_pdesktopsrvdispatcher = new ::remoting_rfb::DesktopSrvDispatcher(m_pgateClientToServer, [this]() { onHappening(); }, m_plogwriter);

         m_pupdatehandlerserver = allocateø ::remoting_rfb::UpdateHandlerServer();
         m_pupdatehandlerserver->initialize_update_handler_server(m_pconfigurator, m_pgateServerToClient, m_pdesktopsrvdispatcher, [this]() { onHappening(); }, m_plogwriter);

         m_puserinputserver = allocateø ::remoting_rfb::UserInputServer(m_pconfigurator, m_pgateServerToClient, m_pdesktopsrvdispatcher, [this]() { onHappening(); }, m_plogwriter);

         m_pconfigserver = allocateø ::remoting_rfb::ConfigServer();
         m_pconfigserver->initialize_config_server(m_pconfigurator, m_pdesktopsrvdispatcher, m_plogwriter);

         m_pgatekickhandler = allocateø ::remoting_rfb::GateKickHandler();
         m_pgatekickhandler->initialize_gate_kick_handler(m_pdesktopsrvdispatcher);

         // Start servers
         m_pdesktopsrvdispatcher->resumeThread();

         // Spy for the session change.
         auto psessionchangeswatcher = createø<::subsystem::SessionChangesWatcher>();
         psessionchangeswatcher->start_SessionChangesWatcher([this]() { onHappening(); }, m_plogwriter);
         // m_psessionchangeswatcher = new SessionChangesWatcher(this, &m_plogwriter);
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Desktop server application failed with error: {}", e.get_message());
         freeResources();
         throw;
      }
   }

   DesktopServerApplication::~DesktopServerApplication()
   {
      m_plogwriter->information("The Desktop server application destructor has been called");
      freeResources();
      m_plogwriter->information("Desktop server application has been terminated");
   }

   void DesktopServerApplication::freeResources()
   {
      try
      {
         if (m_pchannelClientToServer)
            m_pchannelClientToServer->close();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Cannot close client->server channel: {}", e.get_message());
      }
      try
      {
         if (m_pchannelServerToClient)
            m_pchannelServerToClient->close();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Cannot close server->client channel: {}", e.get_message());
      }

      if (m_psessionchangeswatcher)
         delete m_psessionchangeswatcher;

      // This will stop and destroy the dispatcher. So all handles will be
      // unregistered automatically.
      if (m_pdesktopsrvdispatcher)
         delete m_pdesktopsrvdispatcher;

      if (m_pgatekickhandler)
         delete m_pgatekickhandler;
      if (m_pconfigserver)
         delete m_pconfigserver;
      if (m_puserinputserver)
         delete m_puserinputserver;
      if (m_pupdatehandlerserver)
         delete m_pupdatehandlerserver;

      if (m_pgateServerToClient)
         delete m_pgateServerToClient;
      if (m_pgateClientToServer)
         delete m_pgateClientToServer;
      if (m_pchannelServerToClient)
         delete m_pchannelServerToClient;
      if (m_pchannelClientToServer)
         delete m_pchannelClientToServer;
   }

   void DesktopServerApplication::onHappening()
   {
      m_plogwriter->error("An error has been occurred in the desktop server."
                          " Application will be closed.");
      OperatingSystemApplication::shutdown();
   }

   void DesktopServerApplication::onConfigReload(::remoting_rfb_node::ServerConfig *serverConfig) {}

   void DesktopServerApplication::onOperatingSystemApplicationMain()
   {
      try
      {
         
         auto pwallpaperutil = createø<::remoting_rfb::WallpaperUtil>();
         
         pwallpaperutil->initialize_wallpaper_util(m_pconfigurator, m_plogwriter);

         // int retCode = OperatingSystemApplication::onThreadMain();
         OperatingSystemApplication::onOperatingSystemApplicationMain();
         m_plogwriter->information("Desktop server terminated with return code = {}", this->getExitCode());
         // return retCode;
      }
      catch (::exception &e)
      {
         m_plogwriter->error("Desktop server has been terminated with error: {}", e.get_message());
         // return 0;
      }
   }


} // namespace remoting_rfb_node_desktop
 



