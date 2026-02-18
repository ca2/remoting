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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "QueryConnectionApplication.h"
#include "QueryConnectionCommandLine.h"
#include "QueryConnectionDialog.h"
#include "win_system/CurrentConsoleProcess.h"
#include "TvnServerHelp.h"

#include "server_config_lib/Configurator.h"
//#include "win_system/Environment.h"
#include "win_system/Process.h"
#include "win_system/WinCommandLineArgs.h"
#include "util/ResourceLoader.h"
#include "remoting_node/resource.h"
#include "remoting_node_desktop/NamingDefs.h"

QueryConnectionApplication::QueryConnectionApplication(HINSTANCE hInstance,
                                                       const ::scoped_string & scopedstrwindowClassName,
                                                       const ::scoped_string & scopedstrCmdLine)
: LocalWindowsApplication(hInstance, windowClassName),
  m_cmdLine(cmdLine)
{
}

QueryConnectionApplication::~QueryConnectionApplication()
{
}

int QueryConnectionApplication::run()
{
  QueryConnectionCommandLine parser;

  try {
    WinCommandLineArgs cmdArgs(m_cmdLine);
    parser.parse(&cmdArgs);
  } catch (::remoting::Exception &) {
    TvnServerHelp::showUsage();
    return 0;
  }

  ::string peerAddress;

  parser.getPeerAddress(&peerAddress);

  DWORD queryTimeout = parser.isTimeoutSpecified() ? parser.getTimeout() : 30;

  QueryConnectionDialog dialog(peerAddress,
                               parser.isDefaultActionAccept(),
                               queryTimeout);

  return dialog.showModal();
}

int QueryConnectionApplication::execute(const ::scoped_string & scopedstrPeerAddr, bool acceptByDefault, DWORD timeOutSec)
{
   // Prepare command for execution.

  ::string curModulePath;
  ::string command;

  Environment::getCurrentModulePath(&curModulePath);

  command.formatf("{} {} {} {} {} {} {}",
                 curModulePath,
                 QueryConnectionCommandLine::QUERY_CONNECTION,
                 QueryConnectionCommandLine::PEER_ADDR,
                 peerAddr,
                 acceptByDefault ? QueryConnectionCommandLine::ACCEPT : "",
                 QueryConnectionCommandLine::TIMEOUT,
                 timeOutSec);

  LogWriter log(0); // Zero LogWriter.
  Process *process = 0;

  int defaultRetCode = acceptByDefault ? 0 : 1;
  int retCode = defaultRetCode;

  // Run command in separate process.
  Configurator* conf = Configurator::getInstance();
  if (conf->getServiceFlag()) {
    process = new CurrentConsoleProcess(&log, conf->getServerConfig()->getConnectToRdpFlag(), command);
  } else {
    process = new Process(command);
  }

  try {
    process->start();
    process->waitForExit();
    retCode = process->getExitCode();
  } catch (::remoting::Exception &ex) {
    log.error(ex.get_message());
  }

  delete process;

   // If application ret code is unknown then application then return default
   // ret code.
  if (retCode != 0 && retCode != 1) {
    retCode = defaultRetCode;
  }

  return retCode;
}
