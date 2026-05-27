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

//#ifndef __WINEVENTLOG_H__
//#define __WINEVENTLOG_H__

#pragma once

//#include "util/CommonHeader.h"
//#include <vector>
//#include "thread/LocalMutex.h"
//#include "log-writer/LogWriter.h"

namespace remoting
{
   //typedef std::vector<TCHAR *> StringContainer;

   class CLASS_DECL_REMOTING EventLog :
   virtual public ::particle
   {
   public:


      //HANDLE m_hEventLog;
      //LocalMutex m_hEventLogMutex;

      ::subsystem::LogWriter *m_plogwriter;

      //WinEventLog(LogWriter *log);
      EventLog();
      ~EventLog() override;

      // This function enables fully functional this object usage,
      // before that all report will be ignored.
      void enable();

      virtual void reportInfo(::u32 messageId, const_char_pointer fmt, ...);
      virtual void reportWarning(::u32 messageId, const_char_pointer fmt, ...);
      virtual void reportError(::u32 messageId, const_char_pointer fmt, ...);

   //private:
      void reportEvent(::u32 messageId,
                       ::i32 eventType,
                       const_char_pointer fmt,
                     va_list argList);

      //void registerEventSource();
      //void deRegisterEventSource();
      // Updates event sources pathes to the self in the registry.
      //void updateEventSourcesSubkey();

      //HANDLE getLogHandle();

   };

   //#endif // __WINEVENTLOG_H__
} // namespace remoting
