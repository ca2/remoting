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
    * Windows TightVNC server application.
    * Used for running TightVNC server as single windows application.
    */
   class CLASS_DECL_REMOTING_NODE_DESKTOP  ServerApplication : 
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
      // * TightVNC server.
      // */
      //::pointer < Server > m_pserver;
      /**
       * TvnControl application watcher.
       */
      ::pointer < WsConfigRunner > m_pcontrolrunner;

      // NewConnectionEvents *m_newConnectionEvents;

      /**
       * Creates TightVNC server application instance.
       * @param hInstance HINSTANCE of application.
       * @param commaneLine command line string.
       */
      ServerApplication();
      /**
       * Deletes TightVNC server application instance.
       */
      virtual ~ServerApplication();


      virtual void initialize_server_application(
#ifdef WINDOWS
                                                 ::hinstance hInstance, const ::scoped_string &scopedstrwindowClassName,
#endif
                                                 const ::scoped_string &scopedstrCommandLine,
                                                 ::remoting_node::NewConnectionEvents *newConnectionEvents);


      void task_start() override;
      void maintain_task_running_wait_stop_task_signal_and_stop() override;
      void signal_task_stop() override;


      /**
       * Runs TightVNC server windows application.
       *
       * Makes several things:
       *   1) Starts TigthVNC server.
       *   2) Starts TvnControl application.
       *   3) Enters main windows scopedstrMessage loop.
       *   4) Stops TigthVNC server.
       *
       * @return application exit code.
       */
      //virtual int run();
      //void on_server_task_run() override;

      /**
       * Inherited from ServerListener abstact class.
       *
       * Shutdowns WindowsApplication.
       */
      virtual void onServerShutdown();

   //private:
      // This is a callback function that calls when the log can be initialized.
      virtual void onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName, unsigned char logLevel);

      // This is a callback function that calls when log properties have changed.
      virtual void onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel);



   };


} // remoting_node_desktop



