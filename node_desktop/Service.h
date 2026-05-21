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


//#include "Server.h"
//#include "ServerListener.h"
//#include "log-server/LogServer.h"
//#include "log-server/ClientLogWriter.h"
#include "subsystem/node/Service.h"

//#include "subsystem/thread/Thread.h"
#include "remoting/node_desktop/Task.h"
//#include "WinServiceEvents.h"
//#include "NewConnectionEvents.h"
#include "acme/parallelization/happening.h"


namespace remoting_node_desktop
{
   /**
    * Remoting service.
    *
    * Solves problems of installing, reinstalling, removing, starting, stopping remoting_node
    * from service control manager.
    *
    * Also contains body of tvnservice.
    */
   class CLASS_DECL_REMOTING_NODE_DESKTOP Service : 
      virtual public ::subsystem::Service,
      virtual public Task        
   {
   public:

   //protected:
      /**
       * Shutdown service event.
       */
      // ::happening m_shutdownEvent;
      ::happening m_shutdownEvent;
      ///**
      // * Remoting Node.
      // */
      //::pointer < Server > m_pserver;

      // LogServer m_logServer;
      //  ClientLogWriter m_clientLogWriter;
      //::subsystem::LogWriter *m_clientLogWriter;

      ::pointer < ServiceEvents > m_pwinserviceevents;
      /*NewConnectionEvents *m_newConnectionEvents;*/
      /**
       * Command line key which needed to start remoting_node binary as service.
       */
      static ::string_literal SERVICE_COMMAND_LINE_KEY;
   //public:
      /**
       * Creates object.
       */
      //Service(WinServiceEvents *winServiceEvents,
        //         NewConnectionEvents *newConnectionEvents);
      Service();
      /**
       * Deletes object.
       */
      ~Service() override;


      virtual void initialize_remoting_node_desktop_service(ServiceEvents *winServiceEvents, ::remoting_node::NewConnectionEvents *newConnectionEvents);


      void task_start() override;
      void maintain_task_running_wait_stop_task_signal_and_stop() override;
      //void _maintain_task_running() override;
      void signal_task_stop() override;
      ///void task_stop() override;


      /**
       * Inherited from abstract ServerListener class.
       * Shutdows tvnservice using service control manager.
       */
      virtual void onServerShutdown();

      /**
       * Installs remoting_node service.
       * @throws SystemException on fail.
       */
      static void install();
      /**
       * Stops and removes remoting_node service.
       * @throws SystemException when failed to remove service.
       */
      static void remove();
      /**
       * Reinstalls remoting_node service (combite call of remove and install methods).
       * @remark ignores if remove call throws exception.
       * @throws SystemException when fail to install service.
       */
      static void reinstall();
      /**
       * Starts remoting_node service.
       * @param waitCompletion if true, waitThreadToFinish until the status becomes
       *   SERVICE_RUNNING.
       * @throws SystemException on fail.
       */
      static void start(bool waitCompletion = false);
      /**
       * Stopps remoting_node service.
       * @param waitCompletion if true, waitThreadToFinish until the status becomes
       *   SERVICE_STOPPED.
       * @throws SystemException on fail.
       */
      static void stop(bool waitCompletion = false);

   //protected:
      /**
       * Inherited from superclass.
       * Starts remoting_node execution.
       * @throws SystemException when failed to start.
       */
      ////virtual void onStart() override;


      void onServiceMain() override;

      ///void on_server_task_run() override;

      /**
       * Inherited from superclass.
       * TvnService working body.
       */
      //virtual void main() override;

      /**
       * Inherited from superclass.
       * Stops remoting_node execution.
       */
      //virtual void onStop() override;

      /**
       * Creates path to binary of remoting_node service.
       * @param binPath [out] output string where path will be stored.
       * @return true on success, false on error.
       * @remark creates path with all needed keys for starting service.
       */
      static bool getBinPath(::string & binPath);

      // This is a callback function that calls when the log can be initialized.
      virtual void onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName, unsigned char logLevel);

      // This is a callback function that calls when log properties have changed.
      virtual void onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel);


   };


} // namespace remoting_node_desktop



