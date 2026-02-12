// Copyright (C) 2011,2012 GlavSoft LLC.
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
#include "framework.h"
#include "client_config_lib/ViewerConfig.h"
#include "log_writer/LogWriter.h"
#include "remoting_impact.h"
#include "ConnectionData.h"
#include "ConnectionListener.h"
#include "ViewerCmdLine.h"
#include "util/ResourceLoader.h"
#include "acme/platform/system.h"

//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE,
//                       LPTSTR lpCmdLine, int nCmdShow)
int remoting_impact_main()
{

   //new ::platform::system();

   // if (__argv[1])
   // {
   //
   //    if (::file_exists(__argv[1]))
   //    {
   //
   //       ::file::path path = __argv[1];
   //
   //    }
   //
   // }

  ViewerSettingsManager::initInstance(RegistryPaths::VIEWER_PATH);
  SettingsManager *sm = ViewerSettingsManager::getInstance();

  ViewerConfig config(RegistryPaths::VIEWER_PATH);
  config.loadFromStorage(sm);
HINSTANCE hInstance = (HINSTANCE)::system()->m_hinstanceThis;
  ConnectionConfig conConf;
  ConnectionData condata;
  bool isListening = false;
  ViewerCmdLine cmd(&condata, &conConf, &config, &isListening);
  // resource-loader initialization
  ResourceLoader resourceLoader(hInstance);
  try {
    config.initLog(LogNames::LOG_DIR_NAME, LogNames::VIEWER_LOG_FILE_STUB_NAME);
  }  catch (...) {
  }

  try {
    cmd.parse();
  }
  catch (const CommandLineFormatException &exception) {
    StringStorage strError(exception.getMessage());
    MessageBox(0,
      strError.getString(),
      ProductNames::VIEWER_PRODUCT_NAME,
      MB_OK | MB_ICONERROR);
    return 0;
  }
  catch (const CommandLineFormatHelp &) {
    cmd.onHelp();
    return 0;
  } 

  LogWriter logWriter(config.getLogger());


  logWriter.debug(_T("main()"));
  logWriter.debug(_T("loading settings from storage completed"));
  logWriter.debug(_T("Log initialization completed"));
  
  int result = 0;
  try {
    remoting_impact tvnViewer(hInstance,
                        ApplicationNames::WINDOW_CLASS_NAME,
                        WindowNames::TVN_WINDOW_CLASS_NAME);
    if (isListening) {
      // FIXME: set listening connection options.
      tvnViewer.startListening(ConnectionListener::DEFAULT_PORT);
    } else if (!condata.is_empty()) {
      tvnViewer.newConnection(condata, conConf);
    } else {
      tvnViewer.showLoginDialog();
    }
    result = tvnViewer.run();
  } catch (const Exception &ex) {
    MessageBox(0,
               StringTable::getString(IDS_UNKNOWN_ERROR_IN_VIEWER),
               ProductNames::VIEWER_PRODUCT_NAME,
               MB_OK | MB_ICONERROR);
    logWriter.debug(ex.getMessage());
  }

  return result;
}
