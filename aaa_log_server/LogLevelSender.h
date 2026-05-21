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


#include "remoting/thread/Thread.h"
#include "remoting/win_system/::happening.h"
#include "remoting/io/DataOutputStream.h"

class LogLevelSender : public Thread
{
public:
  LogLevelSender();
  virtual ~LogLevelSender();

  // Starts log level sender. If LogLevelSender still was not called then
  // the updateLevel() function calling will be ignored.
  void startSender(OutputStream *outStream);

  // Signals to the self thread for update available.
  void updateLevel(unsigned char newLevel);

private:
  virtual void onThreadMain();
  virtual void onTermThread();

  ::happening m_happeningSleeper;

  OutputStream *m_outStream;
  bool m_updateAvailable;
  unsigned char m_logLevel;
  lockable_critical_section m_criticalsectionUpdate;
};

//// __LOGLEVELSENDER_H__
