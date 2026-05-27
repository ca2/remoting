// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "remoting/io/Channel.h"
#include "remoting/thread/Thread.h"
#include "remoting/win_system/Pipe.h"
#include "LogListener.h"
#include "LogLevelSender.h"

class LogConnAuthListener;

class LogConn : public Thread
{
public:
  LogConn(Channel *channel, LogConnAuthListener *pclientauthlistener,
          LogListener *extLogListener, ::u8 logLevel);
  virtual ~LogConn();

  // This function closes the connection that brings to object
  // termination.
  virtual void close();

  void changeLogLevel(::u8 newLevel);

private:
  virtual void onThreadMain();
  virtual void onTermThread();

  void assignConnection();
  void dispatch();

  LogConnAuthListener *m_extAuthListener;
  LogListener *m_extLogListener;

  Channel *m_serviceChannel;
  Channel *m_logListenChannel;
  Channel *m_levelSendChannel;
  FileAccountHandle m_handle;
  lockable_critical_section m_criticalsectionChannel;

  ::u8 m_logLevel;
  lockable_critical_section m_criticalsectionLogLevel;

  LogLevelSender m_logLevelSender;
};

//// __LOGCONN_H__
