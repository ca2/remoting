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


#include "subsystem/node/OperatingSystemApplication.h"
////#include "subsystem/platform/AnEventListener.h"
#include "subsystem/node/AnonymousPipe.h"
#include "remoting/remoting/desktop_ipc/BlockingGate.h"
#include "remoting/remoting/desktop_ipc/DesktopSrvDispatcher.h"
#include "remoting/remoting/desktop_ipc/UpdateHandlerServer.h"
#include "remoting/remoting/desktop_ipc/UserInputServer.h"
#include "remoting/remoting/desktop_ipc/ConfigServer.h"
#include "remoting/remoting/desktop_ipc/GateKickHandler.h"
#include "subsystem/thread/SessionChangesWatcher.h"
#include "subsystem/node/LocalOperatingSystemApplication.h"
//#include "log-server/ClientLogWriter.h"
//#include "log_writer/LogWriter.h"
#include "remoting/remoting/server_config/ConfigReloadListener.h"
#include "subsystem/platform/CommandLineArguments.h"


namespace remoting_node_desktop
{

   /**
    * Desktop server application.
    */
   class DesktopServerApplication : public ::subsystem::LocalOperatingSystemApplication,
                                    // public AnEventListener,
                                    public ConfigReloadListener
   {
   public:
      /**
       * Initializes desktop server.
       * @param appInstance HINSTANCE of application.
       * @throws ::subsystem::Exception (or SystemException) on fail.
       * @fixme make command line parsing in this class.
       */
      DesktopServerApplication(HINSTANCE appInstance, const ::scoped_string &scopedstrwindowClassName,
                               const ::subsystem::CommandLineArguments *cmdArgs);

      ~DesktopServerApplication() override;

      /**
       * Inherited from superclass.
       */
      virtual void run() override;

   //protected:
      // virtual void onAnObjectEvent();
      virtual void onHappening();
      virtual void onConfigReload(ServerConfig *serverConfig);

   //private:
      void freeResources();

      ::pointer<Configurator> m_pconfigurator;
      //::subsystem::LogWriter *m_clientLogWriter;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      // Transport
      ::subsystem::AnonymousPipe *m_clToSrvChan;
      ::subsystem::AnonymousPipe *m_srvToClChan;
      ::remoting::BlockingGate *m_clToSrvGate;
      ::remoting::BlockingGate *m_srvToClGate;

      ::remoting::DesktopSrvDispatcher *m_dispatcher;

      // Servers
      ::remoting::UpdateHandlerServer *m_updHandlerSrv;
      ::remoting::UserInputServer *m_uiSrv;
      ::remoting::ConfigServer *m_cfgServer;
      ::remoting::GateKickHandler *m_pblockinggateKickHandler;

      ::subsystem::SessionChangesWatcher *m_sessionChangesWatcher;

      unsigned int m_contextSwitchResolution; // in ms
   };


} // namespace remoting_node_desktop





