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
#include "QueryConnectionApplication.h"
#include "QueryConnectionCommandLine.h"
#include "QueryConnectionDialog.h"
#include "subsystem/node/CurrentConsoleProcess.h"
#include "subsystem/node/OperatingSystem.h"
#include "ServerHelp.h"

#include "remoting/remoting_rtc/node_config/Configurator.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/node/Process.h"
#include "subsystem/platform/CommandLineArguments.h"
#include "subsystem/platform/ResourceLoader.h"
#include "remoting/node_desktop/resource.h"
#include "remoting/remoting_rtc/node/NamingDefs.h"
#include "remoting/remoting_rtc/node_config/Configurator.h"

namespace remoting_node_desktop
{
   //QueryConnectionApplication::QueryConnectionApplication(::hinstance hInstance,
   //                                                       const ::scoped_string & scopedstrwindowClassName,
   //                                                       const ::scoped_string & scopedstrCmdLine)
   //: LocalWindowsApplication(hInstance, windowClassName),
   //  m_cmdLine(cmdLine)
   //{
   //}
   QueryConnectionApplication::QueryConnectionApplication() {}

   QueryConnectionApplication::~QueryConnectionApplication()
   {
   }

   void QueryConnectionApplication::initialize_query_connection_application(::remoting_rtc_node::Configurator * pconfigurator,  const ::scoped_string &scopedstrCmdLine)

   {
      m_pconfigurator = pconfigurator;
      initialize_operating_system_application();
      m_cmdLine = scopedstrCmdLine;
   }


   void QueryConnectionApplication::onOperatingSystemApplicationMain()
   {
      QueryConnectionCommandLine parser;

      try {
         //WinCommandLineArgs cmdArgs(m_cmdLine);
         auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_cmdLine);
         //parser.parse(&cmdArgs);
         parser.parse(pcommandlinearguments);
      } catch (::subsystem::Exception &) {
         ServerHelp::showUsage();
         //return 0;
         return;
      }

      ::string peerAddress;

      parser.getPeerAddress(peerAddress);

      DWORD queryTimeout = parser.isTimeoutSpecified() ? parser.getTimeout() : 30;

      QueryConnectionDialog dialog(peerAddress,
                                   parser.isDefaultActionAccept(),
                                   queryTimeout);

      dialog.showModal();

   }


   ::i32 QueryConnectionApplication::run_synchronously(const ::scoped_string & scopedstrPeerAddr, bool acceptByDefault, ::u32 timeOutSec)
   {
      // Prepare command for execution.

      ::string curModulePath;
      ::string command;

      curModulePath = MainSubsystem().OperatingSystem().getCurrentModulePath();

      command.format("{} {} {} {} {} {} {}",
                     curModulePath,
                     QueryConnectionCommandLine::QUERY_CONNECTION,
                     QueryConnectionCommandLine::PEER_ADDR,
                     scopedstrPeerAddr,
                     acceptByDefault ? QueryConnectionCommandLine::ACCEPT : string_literal(""),
                     QueryConnectionCommandLine::TIMEOUT,
                     timeOutSec);

      ::subsystem::LogWriter * plogwriter = nullptr; // Zero ::subsystem::LogWriter.
      ::pointer < ::subsystem::Process > pprocess;

      ::i32 defaultRetCode = acceptByDefault ? 0 : 1;
      ::i32 retCode = defaultRetCode;

      // Run command in separate process.
      ::remoting_rtc_node::Configurator* conf = m_pconfigurator;
      if (conf->getServiceFlag()) {
         auto pprocessNew = createø<::subsystem::CurrentConsoleProcess>();
         pprocessNew->initialize_current_console_process(plogwriter, conf->getServerConfig()->getConnectToRdpFlag(),
                                                         command);
         //pprocess = allocateø CurrentConsoleProcess(&log, conf->getServerConfig()->getConnectToRdpFlag(), command);
         pprocess = pprocessNew;
      } else {
         auto pprocessNew = createø<::subsystem::Process>();
         pprocessNew->initialize_process(command);
         //pprocess = allocateø Process(command);
         pprocess = pprocessNew;
      }

      try {
         pprocess->start();
         pprocess->waitForExit();
         retCode = pprocess->getExitCode();
      } catch (::subsystem::Exception &ex) {
         plogwriter->error(ex.get_message());
      }

      //delete process;
      ::release(pprocess);

      // If application ret code is unknown then application then return default
      // ret code.
      if (retCode != 0 && retCode != 1) {
         retCode = defaultRetCode;
      }

      return retCode;
   }
} // namespace remoting_node_desktop

