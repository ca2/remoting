// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "remoting/util/CommonHeader.h"
#include "io_lib/Channel.h"
#include "io_lib/DataInputStream.h"
#include "io_lib/DataOutputStream.h"
#include "thread/Thread.h"
#include "log_writer/LogWriter.h"
#include "thread/AutoLock.h"
#include "log_writer/LogDump.h"

class ClientLogWriter : public LogWriter, private Thread, public LogDump
{
public:
  // @param logFileName - is a file name for log without extension.
  ClientLogWriter(const ::scoped_string & scopedstrPublicPipeName, const ::scoped_string & scopedstrLogFileName);
  virtual ~ClientLogWriter();

  // The connect() function try connect to a log server. The function
  // was separated from the constructor to connect the server after
  // it's ready (If objects (server and client) will be automated then
  // no guarantees that configuration for service will be valid at them
  // constructors).
  // @throw ::remoting::Exception on an error.
  void connect();

  // Sends log line to the log server.
  virtual void print(int logLevel, const ::scoped_string & scopedstrLine);

  virtual bool acceptsLevel(int logLevel);

private:
  // Writes a log scopedstrMessage.
  virtual void flush(unsigned int processId,
                     unsigned int threadId,
                     const ::earth::time & dt,
                     int level,
                     const ::scoped_string & scopedstrMessage);

  void freeResources();

  virtual void execute();

  int getLogBarrier();
  void setLogBarrier(int newLogBar);

  Channel *m_logSendingChan;
  DataInputStream *m_logInput;
  DataOutputStream *m_logOutput;
  LocalMutex m_logWritingMut;

  Channel *m_levListenChan;

  int m_logBarrier;
  LocalMutex m_logBarMut;

  ::string m_logFileName;
  ::string m_publicPipeName;
};

//// __CLIENTLogWriter_H__
