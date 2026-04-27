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

#include "subsystem/_common_header.h"
//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"

#include "subsystem/platform/CommandLine.h"
#include "subsystem/platform/CommandLineArguments.h"

#include "remoting/node_desktop/Service.h"
#include "remoting/node_desktop/ServerApplication.h"
#include "remoting/node_desktop/QueryConnectionApplication.h"
#include "remoting/node_desktop/DesktopServerApplication.h"
#include "remoting/node_desktop/AdditionalActionApplication.h"
#include "remoting/node_desktop/ServiceControlApplication.h"
#include "remoting/node_desktop/ServiceControlCommandLine.h"
#include "remoting/node_desktop/QueryConnectionCommandLine.h"
#include "remoting/node_desktop/DesktopServerCommandLine.h"

#include "remoting/control_desktop/ControlApplication.h"
#include "remoting/control_desktop/ControlCommandLine.h"

#include "remoting/node_desktop/resource.h"
#include "subsystem/node/CrashHook.h"
#include "remoting/node_desktop/NamingDefs.h"

#include "remoting/node_desktop/EventLogWriter.h"


namespace remoting_node_desktop
{

   void ServerApplication::main_node(const file::path &path)
   {

        ::subsystem::LogWriter * plogwriterPre = ::system();

  // Life time of the sysLog must be greater than a TvnService object
  // because the crashHook uses it but fully functional usage possible
  // only after the TvnService object start.
  EventLogWriter winEventLogWriter(plogwriterPre);
  ::subsystem::CrashHook crashHook(&winEventLogWriter);

  //ResourceLoader resourceLoaderSingleton(hInstance);
// TvnService::SERVICE_COMMAND_LINE_KEY = "-service"
  ::subsystem::CommandLineFormat format[] = {
    { "-service", ::subsystem::NO_ARG },

    { ControlCommandLine::CONFIG_APPLICATION, ::subsystem::NO_ARG },
    { ControlCommandLine::CONFIG_SERVICE, ::subsystem::NO_ARG },
    { ControlCommandLine::SET_CONTROL_PASSWORD, ::subsystem::NO_ARG },
    { ControlCommandLine::SET_PRIMARY_VNC_PASSWORD, ::subsystem::NO_ARG },
    { ControlCommandLine::CHECK_SERVICE_PASSWORDS, ::subsystem::NO_ARG },
    { ControlCommandLine::CONTROL_SERVICE, ::subsystem::NO_ARG },
    { ControlCommandLine::CONTROL_APPLICATION, ::subsystem::NO_ARG },

    { DesktopServerCommandLine::DESKTOP_SERVER_KEY, ::subsystem::NO_ARG },
    { QueryConnectionCommandLine::QUERY_CONNECTION, ::subsystem::NO_ARG },

    { AdditionalActionApplication::LOCK_WORKSTATION_KEY, ::subsystem::NO_ARG },
    { AdditionalActionApplication::LOGOUT_KEY, ::subsystem::NO_ARG },

    { ServiceControlCommandLine::INSTALL_SERVICE },
    { ServiceControlCommandLine::REMOVE_SERVICE },
    { ServiceControlCommandLine::REINSTALL_SERVICE },
    { ServiceControlCommandLine::START_SERVICE },
    { ServiceControlCommandLine::STOP_SERVICE }
  };

  ::subsystem::CommandLine parser;

  // We really don't care about parsing result, we only need the first specified key.

  ::string firstKey("");


  try {
    //::subsystem::CommandLineArguments args(lpCmdLine);
     auto pcommandlinearguments = MainSubsystem().getCurrentProcessCommandLineArguments();
     //parser.parse()
    parser.parse(format,  sizeof(format) / sizeof(::subsystem::CommandLineFormat), pcommandlinearguments);
  } catch (...) {
  }
  parser.getOption(0, &firstKey);

  // Check if need to start additional application that packed into remoting_node.exe.

  //if (firstKey == "".isEqualTo(TvnService::SERVICE_COMMAND_LINE_KEY)) {
      if (firstKey == "-service") {
    Service tvnService;
         tvnService.initialize_remoting_node_desktop_service(&winEventLogWriter, &winEventLogWriter);
    try {
      crashHook.setHklmRoot();
      tvnService.run();
    } catch (::subsystem::Exception &) {
       setExitCode(1);
      //return 1;
    }
    return;
  } else if (firstKey == ControlCommandLine::CONFIG_APPLICATION ||
             firstKey == ControlCommandLine::CONFIG_SERVICE ||
             firstKey == ControlCommandLine::SET_CONTROL_PASSWORD ||
             firstKey == ControlCommandLine::SET_PRIMARY_VNC_PASSWORD ||
             firstKey == ControlCommandLine::CONTROL_SERVICE ||
             firstKey == ControlCommandLine::CONTROL_APPLICATION ||
             firstKey == ControlCommandLine::CHECK_SERVICE_PASSWORDS) {
    crashHook.setGuiEnabled();
    try {
      ::remoting_control_desktop::ControlApplication tvnControl(::system()->m_hinstanceThis,
        WindowNames::WINDOW_CLASS_NAME,
        ::system()->command_line_text());
      return tvnControl.run();
    } catch (::subsystem::Exception &fatalException) {
      auto pmessagebox = message_box({},
        fatalException.get_message(),
        MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
        ::user::e_message_box_ok | ::user::e_message_box_icon_error);
       setExitCode(1);
      return;
    }
  } else if (firstKey == AdditionalActionApplication::LOCK_WORKSTATION_KEY ||
    firstKey == AdditionalActionApplication::LOGOUT_KEY) {
    crashHook.setGuiEnabled();
    try {
      AdditionalActionApplication actionApp;

       //actionApp.initialize_additional_action_application(::system()->m_hinstanceThis,
        //WindowNames::WINDOW_CLASS_NAME,
        actionApp.initialize_additional_action_application(::system()->command_line_text());
      return actionApp.run();
    } catch (::subsystem::SystemException &ex) {
       setExitCode(ex.getErrorCode());
      return ;
    }
  } else if (firstKey == DesktopServerCommandLine::DESKTOP_SERVER_KEY) {
    try {
      crashHook.setHklmRoot();
      //WinCommandLineArgs args(lpCmdLine);
       auto pcommandlinearguments = MainSubsystem().getCurrentProcessCommandLineArguments();
      DesktopServerApplication desktopServerApp(::system()->m_hinstanceThis,
        WindowNames::WINDOW_CLASS_NAME,
        pcommandlinearguments);

      desktopServerApp.run();
       int retCode = desktopServerApp.getExitCode();
       setExitCode(retCode);
      return ;
    } catch (...) {
       setExitCode(1);
      return;
    }
  } else if (firstKey == QueryConnectionCommandLine::QUERY_CONNECTION) {
    crashHook.setGuiEnabled();
    try {
      QueryConnectionApplication app;
       // just to satisfy following function
       auto pconfigurationDummy = new ::remoting::Configurator();
       app.initialize_query_connection_application(//::system()->m_hinstanceThis,
        //WindowNames::WINDOW_CLASS_NAME,
        pconfigurationDummy,
        ::system()->command_line_text());
      return app.run();
    } catch (...) {
       setExitCode(1);
      return;
    }
  } else if (firstKey == ServiceControlCommandLine::INSTALL_SERVICE ||
             firstKey == ServiceControlCommandLine::REMOVE_SERVICE ||
             firstKey == ServiceControlCommandLine::REINSTALL_SERVICE ||
             firstKey == ServiceControlCommandLine::START_SERVICE ||
             firstKey == ServiceControlCommandLine::STOP_SERVICE) {
    crashHook.setGuiEnabled();
    ServiceControlApplication tvnsc(::system()->m_hinstanceThis,
      WindowNames::WINDOW_CLASS_NAME,
      ::system()->command_line_text());
    return tvnsc.run();
  }

  // No additional applications, run TightVNC server as single application.
  crashHook.setGuiEnabled();
  ServerApplication tvnServer;
      tvnServer.initialize_server_application(::system()->m_hinstanceThis,
    WindowNames::WINDOW_CLASS_NAME,
    ::system()->command_line_text(), &winEventLogWriter);

  return tvnServer.run();

   }


} // namespace remoting_node_desktop



// int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//                        LPTSTR lpCmdLine, int nCmdShow)
// {
//   ::subsystem::LogWriter preLog(0);
//
//   // Life time of the sysLog must be greater than a TvnService object
//   // because the crashHook uses it but fully functional usage possible
//   // only after the TvnService object start.
//   WinEventLogWriter winEventLogWriter(&preLog);
//   CrashHook crashHook(&winEventLogWriter);
//
//   ResourceLoader resourceLoaderSingleton(hInstance);
//
//   CommandLineFormat format[] = {
//     { TvnService::SERVICE_COMMAND_LINE_KEY, NO_ARG },
//
//     { ControlCommandLine::CONFIG_APPLICATION, NO_ARG },
//     { ControlCommandLine::CONFIG_SERVICE, NO_ARG },
//     { ControlCommandLine::SET_CONTROL_PASSWORD, NO_ARG },
//     { ControlCommandLine::SET_PRIMARY_VNC_PASSWORD, NO_ARG },
//     { ControlCommandLine::CHECK_SERVICE_PASSWORDS, NO_ARG },
//     { ControlCommandLine::CONTROL_SERVICE, NO_ARG },
//     { ControlCommandLine::CONTROL_APPLICATION, NO_ARG },
//
//     { DesktopServerCommandLine::DESKTOP_SERVER_KEY, NO_ARG },
//     { QueryConnectionCommandLine::QUERY_CONNECTION, NO_ARG },
//
//     { AdditionalActionApplication::LOCK_WORKSTATION_KEY, NO_ARG },
//     { AdditionalActionApplication::LOGOUT_KEY, NO_ARG },
//
//     { ServiceControlCommandLine::INSTALL_SERVICE },
//     { ServiceControlCommandLine::REMOVE_SERVICE },
//     { ServiceControlCommandLine::REINSTALL_SERVICE },
//     { ServiceControlCommandLine::START_SERVICE },
//     { ServiceControlCommandLine::STOP_SERVICE }
//   };
//
//   CommandLine parser;
//
//   // We really don't care about parsing result, we only need the first specified key.
//
//   ::string firstKey("");
//
//
//   try {
//     WinCommandLineArgs args(lpCmdLine);
//     parser.parse(format,  sizeof(format) / sizeof(CommandLineFormat), &args);
//   } catch (...) {
//   }
//   parser.getOption(0, &firstKey);
//
//   // Check if need to start additional application that packed into remoting_node.exe.
//
//   if (firstKey.isEqualTo(TvnService::SERVICE_COMMAND_LINE_KEY)) {
//     TvnService tvnService(&winEventLogWriter, &winEventLogWriter);
//     try {
//       crashHook.setHklmRoot();
//       tvnService.run();
//     } catch (::subsystem::Exception &) {
//       return 1;
//     }
//     return 0;
//   } else if (firstKey.isEqualTo(ControlCommandLine::CONFIG_APPLICATION) ||
//              firstKey.isEqualTo(ControlCommandLine::CONFIG_SERVICE) ||
//              firstKey.isEqualTo(ControlCommandLine::SET_CONTROL_PASSWORD) ||
//              firstKey.isEqualTo(ControlCommandLine::SET_PRIMARY_VNC_PASSWORD) ||
//              firstKey.isEqualTo(ControlCommandLine::CONTROL_SERVICE) ||
//              firstKey.isEqualTo(ControlCommandLine::CONTROL_APPLICATION) ||
//              firstKey.isEqualTo(ControlCommandLine::CHECK_SERVICE_PASSWORDS)) {
//     crashHook.setGuiEnabled();
//     try {
//       ControlApplication tvnControl(hInstance,
//         WindowNames::WINDOW_CLASS_NAME,
//         lpCmdLine);
//       return tvnControl.run();
//     } catch (::subsystem::Exception &fatalException) {
//       MessageBox(0,
//         fatalException.get_message(),
//         MainSubsystem().StringTable().getString(IDS_MBC_TVNCONTROL),
//         ::user::e_message_box_ok | MB_ICONERROR);
//       return 1;
//     }
//   } else if (firstKey.isEqualTo(AdditionalActionApplication::LOCK_WORKSTATION_KEY) ||
//     firstKey.isEqualTo(AdditionalActionApplication::LOGOUT_KEY)) {
//     crashHook.setGuiEnabled();
//     try {
//       AdditionalActionApplication actionApp(hInstance,
//         WindowNames::WINDOW_CLASS_NAME,
//         lpCmdLine);
//       return actionApp.run();
//     } catch (SystemException &ex) {
//       return ex.getErrorCode();
//     }
//   } else if (firstKey.isEqualTo(DesktopServerCommandLine::DESKTOP_SERVER_KEY)) {
//     try {
//       crashHook.setHklmRoot();
//       WinCommandLineArgs args(lpCmdLine);
//       DesktopServerApplication desktopServerApp(hInstance,
//         WindowNames::WINDOW_CLASS_NAME,
//         &args);
//
//       int retCode = desktopServerApp.run();
//       return retCode;
//     } catch (...) {
//       return 1;
//     }
//   } else if (firstKey.isEqualTo(QueryConnectionCommandLine::QUERY_CONNECTION)) {
//     crashHook.setGuiEnabled();
//     try {
//       QueryConnectionApplication app(hInstance,
//         WindowNames::WINDOW_CLASS_NAME,
//         lpCmdLine);
//       return app.run();
//     } catch (...) {
//       return 1;
//     }
//   } else if (firstKey.isEqualTo(ServiceControlCommandLine::INSTALL_SERVICE) ||
//              firstKey.isEqualTo(ServiceControlCommandLine::REMOVE_SERVICE) ||
//              firstKey.isEqualTo(ServiceControlCommandLine::REINSTALL_SERVICE) ||
//              firstKey.isEqualTo(ServiceControlCommandLine::START_SERVICE) ||
//              firstKey.isEqualTo(ServiceControlCommandLine::STOP_SERVICE)) {
//     crashHook.setGuiEnabled();
//     ServiceControlApplication tvnsc(hInstance,
//       WindowNames::WINDOW_CLASS_NAME,
//       lpCmdLine);
//     return tvnsc.run();
//   }
//
//   // No additional applications, run TightVNC server as single application.
//   crashHook.setGuiEnabled();
//   ServerApplication tvnServer(hInstance,
//     WindowNames::WINDOW_CLASS_NAME,
//     lpCmdLine, &winEventLogWriter);
//
//   return tvnServer.run();
// }
