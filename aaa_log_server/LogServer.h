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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "ListenLogServer.h"
#include "LogConn.h"
#include "log_writer/FileAccount.h"
#include "util/Singleton.h"
#include "LogConnAuthListener.h"
#include "thread/ThreadCollector.h"
#include "LogListener.h"
//#include <list>
//#include <map>

typedef ::list_base<LogConn *> ConnList;
typedef ::list_base<LogConn *>::iterator ConnListIter;
typedef ::map<FileAccountHandle, FileAccount *> FileAccountList;
typedef ::map<FileAccountHandle, FileAccount *>::iterator FAccountListIter;

class LogServer : private ConnectionListener, private LogConnAuthListener,
                  private LogListener, private Singleton<LogServer>
{
public:
  LogServer(const ::scoped_string & scopedstrPublicPipeName);
  virtual ~LogServer();

  // The start() function allows to server accept connections. The function
  // was separated from the constructor to allow initialize the configuration
  // before.
  // @throw ::remoting::Exception on an error.
  void start(const ::scoped_string & scopedstrLogDir,
             unsigned char logLevel, size_t headerLineCount);

  void changeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel);

private:
  virtual void onNewConnection(Channel *channel);
  virtual FileAccountHandle  onLogConnAuth(LogConn *logConn, bool success,
                                           const ::scoped_string & scopedstrFileName);
  virtual void onDisconnect(LogConn *logConn);
  virtual void onLog(FileAccountHandle handle,
                     unsigned int processId,
                     unsigned int threadId,
                     const ::earth::time & dt,
                     int level,
                     const ::scoped_string & scopedstrMessage);
  virtual void onAnErrorFromLogConn(const ::scoped_string & scopedstrMessage);

  FileAccountHandle addConnection(const ::scoped_string & scopedstrFileName);

  // Stores all printed lines as a log header and stops it accumulation.
  void storeHeader();

  ::string m_publicPipeName;

  ::string m_logDir;
  unsigned char m_logLevel;
  ConnList m_notAuthConnList;
  ConnList m_connList;
  FileAccountList m_fileAccountList;
  LocalMutex m_logPropsMutex;

  ListenLogServer *m_listenLogServer;

  ThreadCollector m_threadCollector;

  size_t m_headerLineCount;
  size_t m_totalLogLines;
};

//// __LOGSERVER_H__
