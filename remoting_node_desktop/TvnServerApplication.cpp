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

#include "TvnServerApplication.h"
#include "ServerCommandLine.h"
#include "TvnServerHelp.h"

#include "remoting/remoting_common/thread/GlobalMutex.h"

#include "remoting/remoting_common/util/ResourceLoader.h"
#include "remoting/remoting_common/util/StringTable.h"
#include "remoting_node_desktop/NamingDefs.h"
#include "remoting/remoting_common/win_system/WinCommandLineArgs.h"

#include "remoting_node/resource.h"

#include "remoting/remoting_common/win_system/RegistryKey.h"

TvnServerApplication::TvnServerApplication(HINSTANCE hInstance,
                                           const ::scoped_string & scopedstrwindowClassName,
                                           const ::scoped_string & scopedstrCommandLine,
                                           NewConnectionEvents *newConnectionEvents)
: WindowsApplication(hInstance, windowClassName),
  m_fileLogWriter(true),
  m_tvnServer(0),
  m_commandLine(commandLine),
  m_newConnectionEvents(newConnectionEvents)
{
}

TvnServerApplication::~TvnServerApplication()
{
}

int TvnServerApplication::run()
{
  // FIXME: May be an unhandled exception.
  // Check wrong command line and situation when we need to show help.

  try {
    ServerCommandLine parser;
    WinCommandLineArgs cmdArgs(m_commandLine);
    if (!parser.parse(&cmdArgs) || parser.showHelp()) {
      throw ::remoting::Exception("Wrong command line argument");
    }
  } catch (...) {
    TvnServerHelp::showUsage();
    return 0;
  }

  // Reject 2 instances of TightVNC server application.

  GlobalMutex *appInstanceMutex;

  try {
    appInstanceMutex = new GlobalMutex(
      ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME, false, true);
  } catch (...) {
    ::remoting::message_box(0,
               StringTable::getString(IDS_SERVER_ALREADY_RUNNING),
               StringTable::getString(IDS_MBC_TVNSERVER), MB_OK | MB_ICONEXCLAMATION);
    return 1;
  }

  // check the HKLM\SOFTWARE\TightVNC\Server\ has ServiceOnly subsection and exit if found
  // to create the key and set acces rights run the PS script:
  // New-Item -Path HKLM:\SOFTWARE\TightVNC\Server -Name ServiceOnly
  // $ACL = Get-Acl HKLM:\SOFTWARE\TightVNC\Server\ServiceOnly
  // $AccessRule = new-object System.Security.AccessControl.RegistryAccessRule("Users", "ReadKey", "None", "None", "Allow")
  // $ACL.SetAccessRule($AccessRule)
  // $ACL | Set-Acl HKLM:\SOFTWARE\TightVNC\Server\ServiceOnly
  RegistryKey key(HKEY_LOCAL_MACHINE, "SOFTWARE\\TightVNC\\Server\\ServiceOnly", false);
  if (key.isOpened()) {
    ::remoting::message_box(0,
      "Couldn't run the server in Application mode",
      "Server error", MB_OK | MB_ICONEXCLAMATION);
    return 1;
  }

  // Start TightVNC server and TightVNC control application.
  try {
    m_tvnServer = new TvnServer(false, m_newConnectionEvents, this, &m_fileLogWriter);
    m_tvnServer->addListener(this);
    m_tvnControlRunner = new WsConfigRunner(&m_fileLogWriter);

    int exitCode = WindowsApplication::run();

    delete m_tvnControlRunner;
    m_tvnServer->removeListener(this);
    delete m_tvnServer;
    delete appInstanceMutex;
    return exitCode;
  } catch (::exception &e) {
    // FIXME: Move string to resource
    ::string scopedstrMessage;
    scopedstrMessage.formatf("Couldn't run the server: {}", e.get_message());
    ::remoting::message_box(0,
               scopedstrMessage,
               "Server error", MB_OK | MB_ICONEXCLAMATION);
    return 1;
  }
}

void TvnServerApplication::onTvnServerShutdown()
{
  WindowsApplication::shutdown();
}

void TvnServerApplication::onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName,
                                     unsigned char logLevel)
{
  m_fileLogWriter.init(logDir, fileName, logLevel);
  m_fileLogWriter.storeHeader();
}

void TvnServerApplication::onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel)
{
  m_fileLogWriter.changeLogProps(newLogDir, newLevel);
}
