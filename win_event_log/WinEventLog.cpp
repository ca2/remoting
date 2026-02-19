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
#include "framework.h"
#include "WinEventLog.h"
#include "remoting/util/CommonHeader.h"
#include "remoting_node_desktop/NamingDefs.h"
#include "util/Exception.h"
//#include "win_system/Environment.h"
#include "win_system/RegistryKey.h"
#include "thread/AutoLock.h"

WinEventLog::WinEventLog(LogWriter *log)
: m_hEventLog(0),
  m_log(log)
{
}

WinEventLog::~WinEventLog()
{
  deRegisterEventSource();
}

void WinEventLog::enable()
{
  deRegisterEventSource();
  try {
    updateEventSourcesSubkey();
  } catch (::exception &e) {
    m_log->error("Cannot update event sources registry subkey: {}",
               e.get_message());
  }
  registerEventSource();
}

void WinEventLog::registerEventSource()
{
  AutoLock al(&m_hEventLogMutex);
  m_hEventLog = RegisterEventSource(0, LogNames::WIN_EVENT_PROVIDER_NAME);
}

void WinEventLog::deRegisterEventSource()
{
  AutoLock al(&m_hEventLogMutex);
  if (m_hEventLog)
    DeregisterEventSource(m_hEventLog);
  m_hEventLog = 0;
}

void WinEventLog::updateEventSourcesSubkey()
{
  ::string path;
  if (Environment::getCurrentModulePath(&path)) {
    ::string entry("SYSTEM\\CurrentControlSet\\"
                        "services\\eventlog\\Application\\");
    entry += LogNames::WIN_EVENT_PROVIDER_NAME;
    RegistryKey regKey(HKEY_LOCAL_MACHINE, entry);
    regKey.setValueAsInt32("CategoryCount", 0);
    regKey.setValueAsString("CategoryMessageFile", path);
    regKey.setValueAsString("EventMessageFile", path);
    regKey.setValueAsString("ParameterMessageFile", path);
    regKey.setValueAsInt32("TypesSupported",
                           EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE |
                           EVENTLOG_WARNING_TYPE);
  }
}

void WinEventLog::reportInfo(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_INFORMATION_TYPE, fmt, vl);
  va_end(vl);
}

void WinEventLog::reportWarning(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_WARNING_TYPE, fmt, vl);
  va_end(vl);
}

void WinEventLog::reportError(unsigned int messageId, const ::scoped_string & scopedstrFmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_ERROR_TYPE, fmt, vl);
  va_end(vl);
}

#pragma warning(push)
#pragma warning(disable:4996)

void WinEventLog::reportEvent(unsigned int messageId,
                            WORD eventType,
                            const ::scoped_string & scopedstrFmt,
                            va_list argList)
{
  HANDLE hEventLog = getLogHandle();
  if (hEventLog == 0) {
    return;
  }

  // Format the original string.
  int count = _vsctprintf(fmt, argList);
  _ASSERT(count >= 0);
  ::array_base<TCHAR> formattedStringBuff(count + 1);
  TCHAR *formattedString = &formattedStringBuff.front();
  _vstprintf(formattedString, fmt, argList);

  if (ReportEvent(hEventLog,
                  eventType,
                  0, // category
                  messageId,
                  0, // pointer to an User Sid
                  1, // always only one string
                  0, // data size
                  (LPCTSTR *)&formattedString,
                  0 // data
                  ) == 0) {
    ::string errStr;
    Environment::getErrStr("Cannot report an event to the system log",
                           &errStr);
    m_log->error("{}", errStr);
  }
}

#pragma warning(pop)

HANDLE WinEventLog::getLogHandle()
{
  AutoLock al(&m_hEventLogMutex);
  return m_hEventLog;
}
