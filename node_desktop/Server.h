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


#include "subsystem/_common_header.h"

#include "remoting/remoting/desktop/WinServiceDesktopFactory.h"
#include "remoting/remoting/desktop/ApplicationDesktopFactory.h"
#include "RfbClientManager.h"
#include "RfbServer.h"
#include "ExtraRfbServers.h"
#include "ControlServer.h"
#include "ServerListener.h"

//#include "http-server-lib/HttpServer.h"

//#include "subsystem/thread/ZombieKiller.h"
//#include "subsystem/thread/critical_section.h"
//#include "log_writer/LogWriter.h"
#include "subsystem/platform/Singleton.h"
#include "subsystem/platform/ListenerContainer.h"
#include "NewConnectionEvents.h"

#include "remoting/node_desktop/server_config/Configurator.h"

#include "remoting/control_desktop/ServerInfo.h"
#include "LogInitListener.h"

namespace remoting_node_desktop
{
   /**
    * TightVNC server singleton that includes serveral components:
    *   1) Zombie killer singleton.
    *   2) Configurator singleton.
    *   3) Log singleton.
    *   4) Rfb servers (main rfb server and extra servers).
    *   5) Http server.
    *   6) ::innate_subsystem::Control server.
    *   7) Other features:
        1) Do action when last client disconnects.
    */
   //class Server : public Singleton<Server>,
   class  CLASS_DECL_REMOTING_NODE_DESKTOP Server : 
         public ::subsystem::ListenerContainer<ServerListener *>,
         public ConfigReloadListener,
         public RfbClientManagerEventListener
   {
   public:


            ::subsystem::LogWriter *m_plogwriter;
      // ZombieKiller m_zombieKiller;

      ::pointer<Configurator> m_pconfigurator;
      /**
       * Shortcut to global server configuration.
       */
      ::pointer < ServerConfig > m_pserverconfig;

      /**
       * Mutex for protecting servers.
       */
      critical_section m_mutex;

      /**
       * Flag that determitates if we run in server context.
       * true if service, false if application.
       */
      bool m_bRunAsService;

      ::pointer < WinServiceDesktopFactory > m_pservicedesktopfactory;
      ::pointer < ApplicationDesktopFactory > m_papplicationdesktopfactory;
      /**
       * Rfb client manager (for all rfb servers), used by rfb servers
       * rfb clients, control server and control clients.
       */
      ::pointer < RfbClientManager > m_prfbclientmanager;
      /**
       * ::innate_subsystem::Control server.
       */
      ::pointer < ControlServer  > m_pcontrolserver;
      /**
       * Builtin http server.
       */
      // HttpServer *m_httpServer;
      /**
       * Main rfb server.
       */
      ::pointer < RfbServer  > m_prfbserver;
      /**
       * Extra servers for extra ports. This object is not protected by any mutex
       * and it does not implement any internal locking, so it should be used with
       * caution. Here we change its state on owner creation, on owner deletion
       * and on each configuration change (via a listener function called from
       * other threads). The listener function is registered after the object
       * creation and unregistered before the owner destruction, and its calls are
       * properly synchronized. Thus, we can be sure that m_extraRfbServers is not
       * used by different threads simultaneously.
       */
      ::pointer < ExtraRfbServers  > m_pextrarfbservers;

      ::pointer < LogInitListener  > m_ploginitlistener;

      unsigned int m_contextSwitchResolution; // in ms
      /**
       * Creates and starts TightVNC server execution (in separate thread).
       *
       * Makes sereval steps:
       *  1) Instanizes zombie killer.
       *  2) Instanizes configurator and load configuration.
       *  3) Instanizes log.
       *  4) Starts all servers.
       *
       * @param runsInServiceContext must be set to true if Server is running in service context,
       * false, if in context of single application. Parameter determinates control client behavour and
       * initial place for loading TightVNC configuration.
       *
       * @remark doesn't block calling thread execution cause all servers runs in it's own threads.
       * To know when need to shutdown TightVNC server you need to use addListener method.
       */
      Server();
      /**
       * Stops and destroys TightVNC server.
       * @remark don't generate shutdown signal(like shutdown() method does) for listeners.
       */
      ~Server() override;

      /**
       * Fills structure with information of current state of Server.
       * @param info [out] output parameter that will contain TightVNC server information
       * after call of this method.
       * @fixme place extended information to server info.
       */
      virtual void getServerInfo(ServerInfo *info);

      virtual void initialize_remoting_node_desktop_server(bool runsInServiceContext, NewConnectionEvents *newConnectionEvents,
                                    LogInitListener *logInitListener, ::subsystem::LogWriter *plogwriter);

      virtual void on_start();

      /**
       * Inherited from ConfigReloadListener interface to catch configuration reload event.
       *
       * Make several things:
       *  1) Changes log level.
       *  2) Restarts rfb servers.
       *  3) Restarts http server.
       */
      virtual void onConfigReload(ServerConfig *serverConfig);

      /**
       * Only generates shutdown signal (event) for Server listeners.
       *
       * @remark used by ControlClient, when it recieves command to shutdown TightVNC.
       * @remark doesn't stop TightVNC server.
       * @fixme rename this method.
       */
      virtual void generateExternalShutdownSignal();

      /**
       * Checks if TightVNC server runs in service context.
       * @returns true if runs in service context.
       * @deprecated use getServerInfo() instead or move to private.
       */
      virtual bool isRunningAsService() const;

      /**
       * Implemented from RfbClientManagerEventListener.
       *
       * Does nothing.
       */
      virtual void afterFirstClientConnect();

      /**
       * Implemented from RfbClientManagerEventListener.
       *
       * Does specifed in configuration action when last client disconnects from
       * rfb server.
       */
      virtual void afterLastClientDisconnect();

   //protected:
      virtual void restartHttpServer();
      virtual void restartControlServer();
      virtual void restartMainRfbServer();

      virtual void stopHttpServer();
      virtual void stopControlServer();
      virtual void stopMainRfbServer();

      // Calls a callback function to change update log properties.
      virtual void changeLogProps();

   //protected:
      //::subsystem::LogWriter * m_plogwriter;
      ////ZombieKiller m_zombieKiller;

      //::pointer < Configurator > m_pconfigurator;
      ///**
      // * Shortcut to global server configuration.
      // */
      //ServerConfig *m_srvConfig;

      ///**
      // * Mutex for protecting servers.
      // */
      //critical_section m_mutex;

      ///**
      // * Flag that determitates if we run in server context.
      // * true if service, false if application.
      // */
      //bool m_bRunAsService;

      //WinServiceDesktopFactory m_serviceDesktopFactory;
      //ApplicationDesktopFactory m_applicationDesktopFactory;
      ///**
      // * Rfb client manager (for all rfb servers), used by rfb servers
      // * rfb clients, control server and control clients.
      // */
      //RfbClientManager *m_rfbClientManager;
      ///**
      // * ::innate_subsystem::Control server.
      // */
      //ControlServer *m_controlServer;
      ///**
      // * Builtin http server.
      // */
      ////HttpServer *m_httpServer;
      ///**
      // * Main rfb server.
      // */
      //RfbServer *m_rfbServer;
      ///**
      // * Extra servers for extra ports. This object is not protected by any mutex
      // * and it does not implement any internal locking, so it should be used with
      // * caution. Here we change its state on owner creation, on owner deletion
      // * and on each configuration change (via a listener function called from
      // * other threads). The listener function is registered after the object
      // * creation and unregistered before the owner destruction, and its calls are
      // * properly synchronized. Thus, we can be sure that m_extraRfbServers is not
      // * used by different threads simultaneously.
      // */
      //ExtraRfbServers m_extraRfbServers;

      //LogInitListener *m_logInitListener;

      //unsigned int m_contextSwitchResolution; // in ms
   };


} // namespace remoting_node_desktop