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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "EventLog.h"
// #include "util/CommonHeader.h"
// #include "remotingnode-app/NamingDefs.h"
// #include "util/Exception.h"
// #include "win-system/Environment.h"
// #include "win-system/RegistryKey.h"
// #include "thread/AutoLock.h"


namespace remoting_rfb
{
   // EventLog::EventLog(LogWriter *log)
   // : m_hEventLog(0),
   //   m_plogwriter(log)
   // {
   // }


   EventLog::EventLog()
   {


   }


   EventLog::~EventLog()
   {
      //deRegisterEventSource();
   }

   void EventLog::enable()
   {
      // deRegisterEventSource();
      // try {
      //    updateEventSourcesSubkey();
      // } catch (Exception &e) {
      //    m_plogwriter->error("Cannot update event sources registry subkey: %s",
      //               e.getMessage());
      // }
      // registerEventSource();
   }

   // void EventLog::registerEventSource()
   // {
   //    AutoLock al(&m_hEventLogMutex);
   //    m_hEventLog = RegisterEventSource(0, LogNames::WIN_EVENT_PROVIDER_NAME);
   // }
   //
   // void EventLog::deRegisterEventSource()
   // {
   //    AutoLock al(&m_hEventLogMutex);
   //    if (m_hEventLog)
   //       DeregisterEventSource(m_hEventLog);
   //    m_hEventLog = 0;
   // }
   //
   // void EventLog::updateEventSourcesSubkey()
   // {
   //    StringStorage path;
   //    if (Environment::getCurrentModulePath(&path)) {
   //       StringStorage entry("SYSTEM\\CurrentControlSet\\"
   //                           "services\\eventlog\\Application\\");
   //       entry += LogNames::WIN_EVENT_PROVIDER_NAME;
   //       RegistryKey regKey(HKEY_LOCAL_MACHINE, entry.getString());
   //       regKey.setValueAsInt32("CategoryCount", 0);
   //       regKey.setValueAsString("CategoryMessageFile", path.getString());
   //       regKey.setValueAsString("EventMessageFile", path.getString());
   //       regKey.setValueAsString("ParameterMessageFile", path.getString());
   //       regKey.setValueAsInt32("TypesSupported",
   //                              EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE |
   //                              EVENTLOG_WARNING_TYPE);
   //    }
   // }

   void EventLog::reportInfo(::u32 messageId, const char *fmt, ...)
   {
      va_list vl;
      va_start(vl, fmt);
      reportEvent(messageId, e_trace_level_information, fmt, vl);
      va_end(vl);
   }

   void EventLog::reportWarning(::u32 messageId, const char *fmt, ...)
   {
      va_list vl;
      va_start(vl, fmt);
      reportEvent(messageId, e_trace_level_warning, fmt, vl);
      va_end(vl);
   }

   void EventLog::reportError(::u32 messageId, const char *fmt, ...)
   {
      va_list vl;
      va_start(vl, fmt);
      reportEvent(messageId, e_trace_level_error, fmt, vl);
      va_end(vl);
   }

// #pragma warning(push)
// #pragma warning(disable:4996)

   void EventLog::reportEvent(::u32 messageId,
   int eventType,
                               const char *fmt,
                               va_list argList)
   {
      // HANDLE hEventLog = getLogHandle();
      // if (hEventLog == 0) {
      //    return;
      // }

      ::string str;

      str.formatf_arguments(fmt, argList);

      m_plogwriter->trace((enum_trace_level) eventType, str);

      // // Format the original string.
      // int count = _vsctprintf(fmt, argList);
      // ASSERT(count >= 0);
      // std::vector<TCHAR> formattedStringBuff(count + 1);
      // TCHAR *formattedString = &formattedStringBuff.front();
      // _vstprintf(formattedString, fmt, argList);
      //
      // if (ReportEvent(hEventLog,
      //                 eventType,
      //                 0, // category
      //                 messageId,
      //                 0, // pointer to an User Sid
      //                 1, // always only one string
      //                 0, // data size
      //                 (LPCTSTR *)&formattedString,
      //                 0 // data
      //                 ) == 0) {
      //    StringStorage errStr;
      //    Environment::getErrStr("Cannot report an event to the system log",
      //                           &errStr);
      //    m_plogwriter->error("%s", errStr.getString());
      //                 }
   }

// #pragma warning(pop)
//
//    HANDLE EventLog::getLogHandle()
//    {
//       AutoLock al(&m_hEventLogMutex);
//       return m_hEventLog;
//    }
} // namespace remoting_rfb