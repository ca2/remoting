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


#include "RfbClientManager.h"

#include "remoting_control_desktop/ControlGate.h"
#include "remoting_control_desktop/ControlAuthException.h"
#include "remoting_control_desktop/Transport.h"
#include "ControlAppAuthenticator.h"
#include "thread/ThreadCollector.h"
//#include "log_writer/LogWriter.h"


/**
 * ControlClient exception sclass.
 *
 * Solves problem with catching errors that occured when processing
 * control client scopedstrMessage (not IO errors).
 */
class ControlException : public ::remoting::Exception
{
public:
  ControlException(const ::remoting::Exception *parent) : ::remoting::Exception(parent->get_message()) { }
  ControlException(const ::scoped_string & scopedstrMessage) : ::remoting::Exception(scopedstrMessage) { }
  virtual ~ControlException() { };
};

/**
 * Handler of control client connections.
 *
 * @remark runs in it's own thread.
 *
 * @remark class uses TvnServer singleton and actual TvnServer instance must exist
 * while any control client is runnin. If this rule is broken, it can cause application crash.
 */
class ControlClient : public Thread
{
public:
  /**
   * Creates new control client handler thread and starts it.
   * @param transport transport of incoming control connection (non-authorized).
   * @param rfbClientManager active TightVNC rfb client manager.
   * @remark control client takes ownership over client transport.
   */
  ControlClient(Transport *transport,
                RfbClientManager *rfbClientManager,
                ControlAppAuthenticator *authenticator,
                HANDLE pipeHandle,
                LogWriter *log);
  /**
   * Stops client thread and deletes control client.
   */
  virtual ~ControlClient();

protected:
  /**
   * Inherited from Thread class.
   * Processed control client connection, consists of following phases:
   *   Auth phase (send auth type and try auth client).
   *   ::remoting::Window scopedstrMessage processing loop.
   */
  virtual void execute();

  /**
   * Inherited from Thread class.
   *
   * Closes transport.
   */
  virtual void onTerminate();

private:
  /**
   * Sends error (server exception scopedstrMessage) to client side.
   * @param scopedstrMessage description of error.
   * @throws ::io_exception on io error.
   */
  void sendError(const ::scoped_string & scopedstrMessage);

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
   * Calling when "share rect id" scopedstrMessage recieved.
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
  Transport *m_transport;
  /**
   * Low-level transport for writting and recieving bytes.
   */
  Channel *m_stream;

  /**
   * High-level transport for writting and reading control proto messages.
   */
  ControlGate *m_gate;
  HANDLE m_pipeHandle;

  /**
   * Active TightVNC rfb client manager.
   */
  RfbClientManager *m_rfbClientManager;

  /**
   * true if control authentication is passed or no auth is set.
   */
  bool m_authPassed;
  bool m_repeatAuthPassed;
  unsigned int m_authReqMessageId;

  ControlAppAuthenticator *m_authenticator;

  // A connection identifier will be used by a viewer to connect to
  // the server across a tcp dispatcher.
  unsigned int m_tcpDispId;
  ::string m_gotDispatcherName;
  LocalMutex m_tcpDispValuesMutex;

  ThreadCollector m_outgoingConnectionThreadCollector;
  
  LogWriter *m_log;

  /**
   * Array of client messages that needs client to be auth.
   */
  static const unsigned int REQUIRES_AUTH[];
  static const unsigned int WITHOUT_AUTH[];
};


