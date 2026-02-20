// Copyright (C) 2012 GlavSoft LLC.
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


//#include "log_writer/LogWriter.h"
#include "remoting/remoting_common/network/RfbInputGate.h"
#include "remoting/remoting_common/network/RfbOutputGate.h"
#include "remoting/remoting_common/network/socket/SocketIPv4.h"
#include "remoting/remoting_common/network/socket/SocketStream.h"
#include "remoting/remoting_common/thread/LocalMutex.h"
#include "remoting/remoting_common/io/BufferedInputStream.h"

class CLASS_DECL_REMOTING_COMMON TcpConnection
{
public:
  TcpConnection(LogWriter *logWriter);
  virtual ~TcpConnection();

  void bind(const ::scoped_string & scopedstrHost, unsigned short port);
  void bind(SocketIPv4 *socket);
  void bind(RfbInputGate *input, RfbOutputGate *output);

  void connect();
  void close();

  RfbInputGate *getInput() const;
  RfbOutputGate *getOutput() const;
private:
  ::string m_host;
  unsigned short m_port;
  SocketIPv4 *m_socket;
  bool m_socketOwner;
  SocketStream *m_socketStream;
  BufferedInputStream *m_bufInput;
  RfbInputGate *m_input;
  RfbOutputGate *m_output;
  bool m_RfbGatesOwner;

  bool m_wasBound;
  bool m_wasConnected;
  bool m_isEstablished;

  LogWriter *m_logWriter;

  mutable LocalMutex m_connectLock;
};


