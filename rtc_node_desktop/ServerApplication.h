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

#pragma once


//#include "subsystem/_common_header.h"
//#include "subsystem/platform/winhdr.h"
//#include "acme/_operating_system.h"


#include "subsystem/node/OperatingSystemApplication.h"
#include "remoting/node_desktop/Task.h"
//#include "subsystem_node_desktop/Server.h"
//#include "subsystem_node_desktop/ServerListener.h"
//#include "subsystem_node_desktop/WsConfigRunner.h"
//#include "log_writer/FileLogWriter.h"
//#include "subsystem_node_desktop/LogInitListener.h"

namespace remoting_node_desktop
{
   /**
    * Windows Remoting Node application.
    * Used for running Remoting Node as single windows application.
    */
   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP  ServerApplication : 
      virtual public ::subsystem::OperatingSystemApplication,
                             virtual public Task
   {
   public:

            // FileLogWriter m_fileLogWriter;
      //::subsystem::LogWriter * m_fileLogWriter;
      ::pointer < ::subsystem::GlobalMutexInterface > m_pglobalmutexAppInstance;
      /**
       * Command line string.
       */
      ::string m_commandLine;
      ///**
      // * Remoting Node.
      // */
      //::pointer < Server > m_pserver;
      /**
       * TvnControl application watcher.
       */
      ::pointer < WsConfigRunner > m_pcontrolrunner;

      // NewConnectionEvents *m_newConnectionEvents;

      /**
       * Creates Remoting Node application instance.
       * @param hInstance HINSTANCE of application.
       * @param commaneLine command line string.
       */
      ServerApplication();
      /**
       * Deletes Remoting Node application instance.
       */
      virtual ~ServerApplication();


      virtual void initialize_server_application(
#ifdef WINDOWS
                                                 ::hinstance hInstance, const ::scoped_string &scopedstrwindowClassName,
#endif
                                                 const ::scoped_string &scopedstrCommandLine,
                                                 ::remoting_rtc_node::NewConnectionEvents *newConnectionEvents);


      void task_start() override;
      void maintain_task_running_wait_stop_task_signal_and_stop() override;
      void signal_task_stop() override;


      /**
       * Runs Remoting Node windows application.
       *
       * Makes several things:
       *   1) Starts TigthVNC server.
       *   2) Starts TvnControl application.
       *   3) Enters main windows scopedstrMessage loop.
       *   4) Stops TigthVNC server.
       *
       * @return application exit code.
       */
      //virtual ::i32 onThreadMain();
      //void on_server_task_run() override;
      void onOperatingSystemApplicationMain() override;

      /**
       * Inherited from ServerListener abstact class.
       *
       * Shutdowns WindowsApplication.
       */
      virtual void onServerShutdown();

   //private:
      // This is a callback function that calls when the log can be initialized.
      virtual void onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName, ::u8 logLevel);

      // This is a callback function that calls when log properties have changed.
      virtual void onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, ::u8 newLevel);



   };


} // remoting_node_desktop



