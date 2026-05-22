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


#include "RfbClientManager.h"

#include "remoting/rfb_node_desktop/control_desktop/ControlGate.h"
#include "remoting/remoting_rfb/node/ControlAuthException.h"
#include "remoting/rfb_node_desktop/control_desktop/Transport.h"
#include "ControlAppAuthenticator.h"
#include "subsystem/thread/ThreadCollector.h"
//#include "log_writer/LogWriter.h"


namespace remoting_rfb_node_desktop
{

   /**
    * ControlClient exception sclass.
    *
    * Solves problem with catching errors that occured when processing
    * control client scopedstrMessage (not IO errors).
    */
   class ControlException : public ::subsystem::Exception
   {
   public:
      ControlException(const ::subsystem::Exception *parent) : ::subsystem::Exception(parent->get_message()) {}
      ControlException(const ::scoped_string &scopedstrMessage) : ::subsystem::Exception(scopedstrMessage) {}
      virtual ~ControlException() {};
   };

   /**
    * Handler of control client connections.
    *
    * @remark runs in it's own thread.
    *
    * @remark class uses Server singleton and actual Server instance must exist
    * while any control client is runnin. If this rule is broken, it can cause application crash.
    */
   class CLASS_DECL_REMOTING_NODE_DESKTOP ControlClient : public ::subsystem::Thread
   {
   public:
      ::pointer < ::remoting_rfb_node::Configurator > m_pconfigurator;
      /**
       * Creates new control client handler thread and starts it.
       * @param transport transport of incoming control connection (non-authorized).
       * @param rfbClientManager active Remoting rfb client manager.
       * @remark control client takes ownership over client transport.
       */
      ControlClient(::remoting_rfb_node::Configurator * pconfigurator, ::remoting_control_desktop::Transport *transport, RfbClientManager *rfbClientManager, ControlAppAuthenticator *authenticator,
                    ::subsystem::FileInterface *pfilePipeHandle, ::subsystem::LogWriter * plogwriter);
      /**
       * Stops client thread and deletes control client.
       */
      ~ControlClient() override;
      
      void destroy() override;

   protected:
      /**
       * Inherited from Thread class.
       * Processed control client connection, consists of following phases:
       *   Auth phase (send auth type and try auth client).
       *   ::innate_subsystem::Control scopedstrMessage processing loop.
       */
      virtual void onThreadMain();

      /**
       * Inherited from Thread class.
       *
       * Closes transport.
       */
      virtual void onTermThread();

   private:
      /**
       * Sends error (server exception scopedstrMessage) to client side.
       * @param scopedstrMessage description of error.
       * @throws ::io_exception on io error.
       */
      void sendError(const ::scoped_string &scopedstrMessage);

      /**
       * Called when auth scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void authMsgRcdv();

      /**
       * Handlers of control proto messages.
       */

      /**
       * Called when get client ::list_base scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void getClientsListMsgRcvd();
      /**
       * Called when get server info scopedstrMessage reciveved.
       * @throws ::io_exception on io error.
       */
      void getServerInfoMsgRcvd();
      /**
       * Called when reload configuration scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       * @deprecated.
       */
      void reloadConfigMsgRcvd();
      /**
       * Called when disconnect all clients scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void disconnectAllMsgRcvd();
      /**
       * Called when shutdown scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void shutdownMsgRcvd();
      /**
       * Called when add new client scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void addClientMsgRcvd();
      /**
       * Called when Connect to a tcp dispatcher scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void connectToTcpDispatcher();
      /**
       * Called when set server config scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void setServerConfigMsgRcvd();
      /**
       * Called when get server config scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void getServerConfigMsgRcvd();
      /**
       * Called when "get show tray icon flag" scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void getShowTrayIconFlagMsgRcvd();
      /**
       * Called when "update tvncontrol process id" scopedstrMessage recieved.
       * @throws ::io_exception on io error.
       */
      void updateTvnControlProcessIdMsgRcvd();
      /**
       * Calling when "share primary id" scopedstrMessage recieved.
       */
      void sharePrimaryIdMsgRcvd();
      /**
       * Calling when "share display id" scopedstrMessage recieved.
       */
      void shareDisplayIdMsgRcvd();
      /**
       * Calling when "share window id" scopedstrMessage recieved.
       */
      void shareWindowIdMsgRcvd();
      /**
       * Calling when "share rectangle id" scopedstrMessage recieved.
       */
      void shareRectIdMsgRcvd();
      /**
       * Calling when "share full id" scopedstrMessage recieved.
       */
      void shareFullIdMsgRcvd();
      /**
       * Calling when "share app id" scopedstrMessage recieved.
       */
      void shareAppIdMsgRcvd();

   private:
      /**
       * Client transport.
       */
      ::pointer < ::remoting_control_desktop::Transport > m_ptransport;
      /**
       * Low-level transport for writting and recieving bytes.
       */
      ::pointer < Channel > m_pchannel;

      /**
       * High-level transport for writting and reading control proto messages.
       */
      ::pointer < ::remoting_control_desktop::ControlGate > m_pcontrolgate;
      ::pointer<::subsystem::FileInterface> m_pfilePipeHandle;

      /**
       * Active Remoting rfb client manager.
       */
      ::pointer < RfbClientManager > m_prfbclientmanager;

      /**
       * true if control authentication is passed or no auth is set.
       */
      bool m_authPassed;
      bool m_repeatAuthPassed;
      ::u32 m_authReqMessageId;

      ::pointer <ControlAppAuthenticator >m_pcontrolappauthenticator;

      // A connection identifier will be used by a viewer to connect to
      // the server across a tcp dispatcher.
      ::u32 m_uTcpDispId;
      ::string m_strGotDispatcherName;
      lockable_critical_section m_criticalsectionTcpDispValues;

      ::pointer<::subsystem::ThreadCollector> m_pthreadcollectorOutgoingConnection;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;

      /**
       * Array of client messages that needs client to be auth.
       */
      static const ::u32 REQUIRES_AUTH[];
      static const ::u32 WITHOUT_AUTH[];
   };


} // namespace remoting_rfb_node_desktop





