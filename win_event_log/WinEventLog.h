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


#include "remoting/remoting_common/util/CommonHeader.h"
//#include <vector>
#include "remoting/remoting_common/thread/LocalMutex.h"
//#include "log_writer/LogWriter.h"

typedef ::array_base<TCHAR *> StringContainer;

class WinEventLog
{
public:
  WinEventLog(LogWriter *log);
  virtual ~WinEventLog();

  // This function enables fully functional this object usage,
  // before that all report will be ignored.
  void enable();

  void reportInfo(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...);
  void reportWarning(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...);
  void reportError(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...);

private:
  void reportEvent(unsigned int messageId,
                   WORD eventType,
                   const ::scoped_string & scopedstrFmt,
                   va_list argList);

  void registerEventSource();
  void deRegisterEventSource();
  // Updates event sources pathes to the self in the registry.
  void updateEventSourcesSubkey();

  HANDLE getLogHandle();

  HANDLE m_hEventLog;
  LocalMutex m_hEventLogMutex;

  LogWriter *m_log;
};

//// __WINEVENTLOG_H__
