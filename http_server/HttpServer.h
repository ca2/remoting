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


#include "remoting/remoting_common/util/CommonHeader.h"
#include "remoting/remoting_common/network/TcpServer.h"
//#include "log_writer/LogWriter.h"

/**
 * Simple tcp server that accepts connections and give management over
 * incoming connections to HttpClient class.
 */
class HttpServer : public TcpServer
{
public:
  /**
   * Creates and starts http server.
   * @param bindHost host to bind.
   * @param bindPort port bind.
   * @param lockAddr determinates if server must set exclusive address.
   * @throws ::remoting::Exception on fail.
   */
  HttpServer(const ::scoped_string & scopedstrBindHost,
             unsigned short bindPort,
             bool lockAddr,
             LogWriter *log);
  /**
   * Stops http server thread and deletes http server.
   */
  virtual ~HttpServer();

protected:
  /**
   * Inherited from superclass.
   * Give management over incoming connection to new HttpClientThread instance.
   */
  virtual void onAcceptConnection(SocketIPv4 *socket);

private:
  LogWriter *m_log;
};


