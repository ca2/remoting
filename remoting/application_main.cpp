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
//#include "log_writer/LogWriter.h"
#include "application.h"
#include "remoting_impact.h"
#include "ConnectionData.h"
#include "ConnectionListener.h"
#include "ViewerCmdLine.h"
#include "util/ResourceLoader.h"
#include "acme/platform/system.h"
#include "acme/filesystem/filesystem/file_context.h"
#include "remoting/common/remoting.h"
//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE,
//                       LPTSTR lpCmdLine, int nCmdShow)


HMODULE GetModuleFromFunction(void* pFunc)
{
   HMODULE hModule = NULL;

   GetModuleHandleEx(
       GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
       (LPCTSTR)pFunc,
       &hModule
   );

   return hModule;
}

HINSTANCE remoting_impact_hinstance()
{

   HINSTANCE hInstance = (HINSTANCE)GetModuleFromFunction(&remoting_impact_hinstance);

   return hInstance;
}

namespace remoting_remoting {
int application::remoting_impact_main( const ::file::path & path)
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

   ::remoting::ViewerConfig config(RegistryPaths::VIEWER_PATH);
   config.loadFromStorage(sm);
   HINSTANCE hInstance = remoting_impact_hinstance();
   ConnectionConfig conConf;
   ConnectionData condata;
   bool isListening = false;
   //::string strHost;
   auto ini = file()->get_ini(path);

   ::string strHost =ini["host"];
   conConf.enableFullscreen(true);
   conConf.requestShapeUpdates(true);
   conConf.ignoreShapeUpdates(false);

   if (strHost.has_character())
   {
      condata.setHost(wstring(strHost).c_str());
      m_connectingdialog.set_host(strHost);
      m_connectingdialog.set_status("Initiating connection...");
      m_connectingdialog.set_phase1();

   }
   //ViewerCmdLine cmd(&condata, &conConf, &config, &isListening);
   // resource-loader initialization
   ResourceLoader resourceLoader(hInstance);
   try {
      config.initLog(LogNames::LOG_DIR_NAME, LogNames::VIEWER_LOG_FILE_STUB_NAME);
   }  catch (...) {
   }

   // try {
   //   cmd.parse();
   // }
   // catch (const CommandLineFormatException &exception) {
   //   ::string strError(exception.get_message());
   //   ::remoting::message_box(0,
   //     strError,
   //     ProductNames::VIEWER_PRODUCT_NAME,
   //     MB_OK | MB_ICONERROR);
   //   return 0;
   // }
   // catch (const CommandLineFormatHelp &) {
   //   //cmd.onHelp();
   //   return 0;
   // }

   m_logWriter = config.getLogWriter();


   m_logWriter->debug("main()");
   m_logWriter->debug("loading settings from storage completed");
   m_logWriter->debug("Log initialization completed");

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
   } catch (const ::remoting::Exception &ex) {
      ::remoting::message_box(0,
                 StringTable::getString(IDS_UNKNOWN_ERROR_IN_VIEWER),
                 ProductNames::VIEWER_PRODUCT_NAME,
                 MB_OK | MB_ICONERROR);
      m_logWriter->debug(ex.get_message());
   }

   return result;
}


} // namespace remoting_remoting



