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
#include "framework.h"
#include "Service.h"
#include "subsystem/node/OperatingSystem.h"
#include "ServerCommandLine.h"
#include "remoting/node_desktop/NamingDefs.h"
#include "remoting/node_desktop/NewConnectionEvents.h"
#include "remoting/node_desktop/WinServiceEvents.h"
#include "subsystem/node/ServiceControlManagerClient.h"
#include "subsystem/node/OperatingSystem.h"


namespace remoting_node_desktop
{
   ::string_literal Service::SERVICE_COMMAND_LINE_KEY = "-service";

   Service::Service()
   {
      m_bService = true;
      initialize_service(ServiceNames::SERVICE_NAME);
   }

   Service::~Service()
   {
   }


   void Service::initialize_remoting_node_desktop_service(WinServiceEvents * pwinserviceevents,
                                                          NewConnectionEvents *pnewconnectionevents)
   {

      m_pwinserviceevents = pwinserviceevents;
      m_pnewconnectionevents = pnewconnectionevents;
      // m_logServer(LogNames::LOG_PIPE_PUBLIC_NAME),
      // m_clientLogWriter(LogNames::LOG_PIPE_PUBLIC_NAME, LogNames::SERVER_LOG_FILE_STUB_NAME)
      
   }
      
    
   void Service::task_start()
   {

      try 
      {
      
         m_pwinserviceevents->enable();

         _start();
         
         m_pwinserviceevents->onSuccServiceStart();

      }
      catch (::exception &e) 
      {

         m_pwinserviceevents->onFailedServiceStart(e.get_message());

      }

   }

   
   void Service::maintain_task_running_wait_stop_task_signal_and_stop()
   {

      m_shutdownEvent.wait();

      _stop();

      m_pwinserviceevents->onServiceStop();

   }


   void Service::signal_task_stop()
   {

      m_shutdownEvent.set_happening();

   }


   void Service::onServerShutdown()
   {
      Service::stop();
   }

   void Service::install()
   {
      ::string binPath;

      Service::getBinPath(binPath);

      ::subsystem::ServiceControlManagerClient scManager;

      scManager.installService(ServiceNames::SERVICE_NAME,
                               ServiceNames::SERVICE_NAME_TO_DISPLAY,
                               binPath, "");
   }

   void Service::remove()
   {
      ::subsystem::ServiceControlManagerClient scManager;

      scManager.removeService(ServiceNames::SERVICE_NAME);
   }

   void Service::reinstall()
   {
      try {
         remove();
      } catch (...) { }

      Service::install();
   }

   void Service::start(bool waitCompletion)
   {
      ::subsystem::ServiceControlManagerClient scManager;

      scManager.startService(ServiceNames::SERVICE_NAME, waitCompletion);
   }

   void Service::stop(bool waitCompletion)
   {
      ::subsystem::ServiceControlManagerClient scManager;

      scManager.stopService(ServiceNames::SERVICE_NAME, waitCompletion);
   }

   bool Service::getBinPath(::string & binPath)
   {
      ::string pathToServiceBinary;


      pathToServiceBinary = MainSubsystem().OperatingSystem().getCurrentModulePath();
      // // Get executable folder first.
      // if (!) {
      //   return false;
      // }

      // Create formatted binary path.
      binPath.format("\"{}\" {}",
                      pathToServiceBinary,
                      SERVICE_COMMAND_LINE_KEY);

      return true;
   }

   void Service::onLogInit(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName,
                              unsigned char logLevel)
   {
      //  size_t headerLineCount = m_clientLogWriter.getLogDumpSize();
      int headerLineCount = 0;
      //m_logServer.start(scopedstrLogDir, logLevel, headerLineCount);
      //m_clientLogWriter.connect();
   }

   void Service::onChangeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel)
   {
      //m_logServer.changeLogProps(newLogDir, newLevel);
   }
} // namespace remoting_node_desktop