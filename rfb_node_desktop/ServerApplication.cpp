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
#include "ServerApplication.h"
#include "ServerCommandLine.h"
#include "ServerHelp.h"
#include "WsConfigRunner.h"
#include "subsystem/thread/GlobalMutex.h"

#include "subsystem/platform/ResourceLoader.h"
#include "subsystem/platform/StringTable.h"
#include "remoting/remoting_rfb/node/NamingDefs.h"
#include "subsystem/platform/CommandLineArguments.h"

#include "resource.h"
#include "subsystem/platform/Registry.h"

#include "subsystem/platform/RegistryKey.h"

namespace remoting_rfb_node_desktop
{
   //ServerApplication::ServerApplication(HINSTANCE hInstance,
   //                                           const ::scoped_string & scopedstrwindowClassName,
   //                                           const ::scoped_string & scopedstrCommandLine,
   //                                           NewConnectionEvents *newConnectionEvents)
   ////: WindowsApplication(hInstance, windowClassName),
   //  //m_fileLogWriter(true),
   //:  m_tvnServer(0),
   //  m_commandLine(scopedstrCommandLine),
   //  m_newConnectionEvents(newConnectionEvents)
   //{
   //   initialize_operating_system_application();
   //}
    ServerApplication::ServerApplication()
   {
       m_bService = false;
      initialize_operating_system_application();
   }

   ServerApplication::~ServerApplication()
   {
   }


      void ServerApplication::initialize_server_application(
#ifdef WINDOWS
                                                            ::hinstance hInstance, const ::scoped_string &scopedstrwindowClassName,
#endif
                                        const ::scoped_string &scopedstrCommandLine,
                                        ::remoting_rfb_node::NewConnectionEvents *newConnectionEvents)
       //: WindowsApplication(hInstance, windowClassName),
       // m_fileLogWriter(true),
       //: m_tvnServer(0), m_commandLine(scopedstrCommandLine), m_newConnectionEvents(newConnectionEvents)
   {

         initialize_remoting_node_desktop_server_task(newConnectionEvents);
         m_commandLine = scopedstrCommandLine;
         
         
            
   }


   void ServerApplication::task_start()
   {

      // FIXME: May be an unhandled exception.
      // Check wrong command line and situation when we need to show help.

      try
      {
         ServerCommandLine parser;

         auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_commandLine);
         /// WinCommandLineArgs cmdArgs(m_commandLine);
         if (!parser.parse(pcommandlinearguments) || parser.showHelp())
         {
            throw ::subsystem::Exception("Wrong command line argument");
         }
      }
      catch (...)
      {
         ServerHelp::showUsage();
         setExitCode(0);
         return;
      }

      // Reject 2 instances of Remoting Node application.

      //::subsystem::GlobalMutex *appInstanceMutex;

      try
      {
         m_pglobalmutexAppInstance = createø <::subsystem::GlobalMutexInterface>();

         m_pglobalmutexAppInstance->initialize_global_mutex(::remoting_rfb_node::ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME, false, true);
      }
      catch (...)
      {
         MainSubsystem().message_box({}, MainSubsystem().StringTable().getString(IDS_SERVER_ALREADY_RUNNING),
                                     MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER),
                                     ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
         setExitCode(1);
         return;
      }

      // // check the HKLM\SOFTWARE\Remoting\Server\ has ServiceOnly subsection and exit if found
      // // to create the key and set acces rights onThreadMain the PS script:
      // // New-Item -Path HKLM:\SOFTWARE\Remoting\Server -Name ServiceOnly
      // // $ACL = Get-Acl HKLM:\SOFTWARE\Remoting\Server\ServiceOnly
      // // $AccessRule = new-object System.Security.AccessControl.RegistryAccessRule("Users", "ReadKey", "None", "None",
      // // "Allow") $ACL.SetAccessRule($AccessRule) $ACL | Set-Acl HKLM:\SOFTWARE\Remoting\Server\ServiceOnly
      // ::subsystem::RegistryKey key(MainSubsystem().Registry().getLocalMachineKey(),
      //                              "SOFTWARE\\Remoting\\Server\\ServiceOnly", false);
      // if (key.isOpened())
      // {
      //    MainSubsystem().message_box({}, "Couldn't onThreadMain the server in Application mode", "Server error",
      //                                ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
      //
      //    setExitCode(1);
      //    return;
      // }

      // Start Remoting Node and Remoting control application.
      try
      {
         _start();
         //m_tvnServer = new Server(false, m_newConnectionEvents, this, m_fileLogWriter);
         //m_tvnServer->addListener(this);

         bool bRunControlClient = false;

         if (bRunControlClient)
         {

            construct_newø(m_pcontrolrunner);

            m_pcontrolrunner->initialize_ws_config_runner(m_plogwriter);

         }

         //OperatingSystemApplication::onThreadMain();

         //// delete m_tvnControlRunner;
         //m_tvnServer->removeListener(this);
         //// delete m_tvnServer;
         //// delete appInstanceMutex;
         //// return exitCode;
      }
      catch (::exception &e)
      {
         // FIXME: Move string to resource
         ::string scopedstrMessage;
         scopedstrMessage.format("Couldn't onThreadMain the server: {}", e.get_message());
         MainSubsystem().message_box({}, scopedstrMessage, "Server error",
                                     ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);

         setExitCode(1);
         return;
      }

   }


   //void ServerApplication::onThreadMain()
   //{
   //   // FIXME: May be an unhandled exception.
   //   // Check wrong command line and situation when we need to show help.

   //   try {
   //      ServerCommandLine parser;

   //      auto pcommandlinearguments = MainSubsystem().getCommandLineArguments(m_commandLine);
   //      ///WinCommandLineArgs cmdArgs(m_commandLine);
   //      if (!parser.parse(pcommandlinearguments) || parser.showHelp()) {
   //         throw ::subsystem::Exception("Wrong command line argument");
   //      }
   //   } catch (...) {
   //      ServerHelp::showUsage();
   //      setExitCode(0);
   //      return;
   //   }

   //   // Reject 2 instances of Remoting Node application.

   //   ::subsystem::GlobalMutex *appInstanceMutex;

   //   try {
   //      appInstanceMutex = new ::subsystem::GlobalMutex;

   //      appInstanceMutex->initialize_global_mutex(
   //       ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME, false, true);
   //   } catch (...) {
   //      MainSubsystem().message_box({},
   //                 MainSubsystem().StringTable().getString(IDS_SERVER_ALREADY_RUNNING),
   //                 MainSubsystem().StringTable().getString(IDS_MBC_TVNSERVER), ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);
   //      setExitCode(1);
   //      return;
   //   }

   //   // check the HKLM\SOFTWARE\Remoting\Server\ has ServiceOnly subsection and exit if found
   //   // to create the key and set acces rights onThreadMain the PS script:
   //   // New-Item -Path HKLM:\SOFTWARE\Remoting\Server -Name ServiceOnly
   //   // $ACL = Get-Acl HKLM:\SOFTWARE\Remoting\Server\ServiceOnly
   //   // $AccessRule = new-object System.Security.AccessControl.RegistryAccessRule("Users", "ReadKey", "None", "None", "Allow")
   //   // $ACL.SetAccessRule($AccessRule)
   //   // $ACL | Set-Acl HKLM:\SOFTWARE\Remoting\Server\ServiceOnly
   //   ::subsystem::RegistryKey key(MainSubsystem().Registry().getLocalMachineKey(), "SOFTWARE\\Remoting\\Server\\ServiceOnly", false);
   //   if (key.isOpened()) {
   //      MainSubsystem().message_box({},
   //        "Couldn't onThreadMain the server in Application mode",
   //        "Server error", ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);

   //      setExitCode(1);
   //      return;
   //   }

   //   // Start Remoting Node and Remoting control application.
   //   try {
   //      m_tvnServer = new Server(false, m_newConnectionEvents, this, m_fileLogWriter);
   //      m_tvnServer->addListener(this);
   //      m_tvnControlRunner = new WsConfigRunner(m_fileLogWriter);

   //      OperatingSystemApplication::onThreadMain();

   //      //delete m_tvnControlRunner;
   //      m_tvnServer->removeListener(this);
   //      //delete m_tvnServer;
   //      //delete appInstanceMutex;
   //      //return exitCode;
   //   } catch (::exception &e) {
   //      // FIXME: Move string to resource
   //      ::string scopedstrMessage;
   //      scopedstrMessage.format("Couldn't onThreadMain the server: {}", e.get_message());
   //      MainSubsystem().message_box({},
   //                 scopedstrMessage,
   //                 "Server error", ::user::e_message_box_ok | ::user::e_message_box_icon_exclamation);

   //      setExitCode(1);
   //      return;
   //   }
   //}


   void ServerApplication::maintain_task_running_wait_stop_task_signal_and_stop()
   {

      //OperatingSystemApplication::onOp();
      onOperatingSystemApplicationMain();

      //      //delete m_tvnControlRunner;
      //      m_tvnServer->removeListener(this);
      //      //delete m_tvnServer;

      _stop();

   }

   void ServerApplication::onOperatingSystemApplicationMain()
   {

      ::get_task()->run_main_loop();

   }


   void ServerApplication::signal_task_stop()
   {

      ::get_task()->set_finish();

   }


   void ServerApplication::onServerShutdown()
   {
      OperatingSystemApplication::shutdown();
   }

   void ServerApplication::onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName,
                                        unsigned char logLevel)
   {
      //m_fileLogWriter.init(logDir, fileName, logLevel);
      //m_fileLogWriter.storeHeader();
   }

   void ServerApplication::onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel)
   {
      //m_fileLogWriter.changeLogProps(newLogDir, newLevel);
   }
} // namespace remoting_rfb_node_desktop
