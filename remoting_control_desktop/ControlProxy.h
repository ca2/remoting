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


#include "remoting_control_desktop/ControlGate.h"
#include "remoting_control_desktop/RfbClientInfo.h"
#include "remoting_control_desktop/TvnServerInfo.h"

#include "server_config_lib/ServerConfig.h"

#include "ControlMessage.h"
#include "RemoteException.h"

//#include <list>



/**
 * Proxy to some of TightVNC server methods, supported by control protocol.
 * Used to execute remote commands on on TightVNC server.
 * remote errors.
 */
class ControlProxy
{
public:
  /**
   * Creates proxy.
   * @param gate transport to send and recieve messages.
   */
  ControlProxy(ControlGate *gate);
  /**
   * Class destructor.
   */
  virtual ~ControlProxy();

  // If server requests the authentication then firstly will be used
  // *passwordFile parameter then getPassFromConfigEnabled. Call this function
  // to determine this parameters. If this function has not been called then
  // on server auth request will be used a dialog box.
  void setPasswordProperties(const ::scoped_string & scopedstrPasswordFile,
                             bool getPassFromConfigEnabled,
                             bool forService);

  /**
   * Returns rfb server info status.
   * @return rfb server info status.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  TvnServerInfo getServerInfo();

  /**
   * Gets rfb client ::list_base.
   * @param clients [out] output parameters to retrieve info of clients.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void getClientsList(::list_base<RfbClientInfo *> *clients);

  /**
   * Reloads rfb server configuration.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   * @deprecated.
   */
  void reloadServerConfig();
  
  /**
   * Disconnects all existing rfb clients from server.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void disconnectAllClients();

  /**
   * Shutdowns TightVNC server.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void shutdownTightVnc();

  /**
   * Initialized outgoing rfb connection.
   * @param connectString connect string in host[:(port|diplay)] format.
   * @param viewOnly if rfb connection must be in view only mode.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void makeOutgoingConnection(const ::scoped_string & scopedstrConnectString, bool viewOnly);

  /**
   * Initialized connection to a TcpDispatcher.
   * @param connectString connect string in host[:(port|diplay)] format.
   */
  void makeTcpDispatcherConnection(const ::scoped_string & scopedstrConnectString,
                                   const ::scoped_string & scopedstrDispatcherName,
                                   const ::scoped_string & scopedstrKeyword,
                                   unsigned int connectionId);

  // Share only primary display for all clients.
  void sharePrimary();

  // Share only the display for all clients.
  void shareDisplay(unsigned char displayNumber);

  // Share a rect that constrained by a window form.
  // shareWindowName - is a part of the window header name.
  void shareWindow(const ::scoped_string & shareWindowName);

  // Share only a rect.
  void shareRect(const ::int_rectangle &  shareRect);

  // Share full desktop for all clients.
  void shareFull();

  // Share only application region for all clients. Other regions must be painted to black.
  void shareApp(unsigned int procId);

  /**
   * Sends new configuration to server.
   * @param config new server configuration.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void setServerConfig(ServerConfig *config);

  /**
   * Gets current configuration from server.
   * @param config [out] output parameter where configuration will be stored.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void getServerConfig(ServerConfig *config);

  /**
   * Checks if TvnControl must show icon in tray.
   * @throws ::io_exception on io error, RemoteException on error on server side.
   */
  bool getShowTrayIconFlag();

  /**
   * Updates TvnControl application process id on server side.
   * @param processId tvncontrol application process id.
   * @throws RemoteException on error on server.
   * @throws ::io_exception on io error.
   */
  void updateTvnControlProcessId(DWORD processId);

protected:
  /**
   * Returns control scopedstrMessage to write.
   * @param messageId control scopedstrMessage id.
   */
  ControlMessage *createMessage(DWORD messageId);

protected:
  /**
   * Transport for sending and recieving control proto messages.
   */
  ControlGate *m_gate;
  /**
   * Current control scopedstrMessage.
   */
  ControlMessage *m_message;
private:
  /**
   * Deletes control scopedstrMessage created by createMessage() method
   * if it's exists.
   */
  void releaseMessage();

  ::string m_passwordFile;
  bool m_getPassFromConfigEnabled;
  bool m_forService;
};


