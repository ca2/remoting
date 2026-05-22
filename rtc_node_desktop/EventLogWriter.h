// Copyright (C) 2011,2012 GlavSoft LLC.
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


#include "remoting/remoting_rtc/node/NewConnectionEvents.h"
//#include "win-event-log/WinEventLog.h"
#include "subsystem/node/ApplicationCrashEvents.h"
#include "ServiceEvents.h"
#include "remoting/remoting_rtc/event_log/EventLog.h"

namespace remoting_node_desktop
{
   // This class provide response to events by writing to the Windows log.
   class CLASS_DECL_REMOTING_RTC_NODE_DESKTOP  EventLogWriter : public ::remoting_rtc_node::NewConnectionEvents,
                             public ::subsystem::ApplicationCrashEvents,
                             public ServiceEvents
   {
   public:
      EventLogWriter(::subsystem::LogWriter * plogwriter);
      ~EventLogWriter();

      // This function enables fully functional this object usage,
      // before that all report will be ignored.
      virtual void enable();

      // The NewConnectionEvents implementations.
      virtual void onSuccAuth(const ::scoped_string & ip);
      virtual void onAuthFailed(const ::scoped_string & ip);
      virtual void onDisconnect(const ::scoped_string & scopedstrMessage);

      // The ApplicationCrashEvents implementations.
      virtual void onCrash(const ::scoped_string & dumpPath);

      // The WinServiceEvents implementations.
      virtual void onSuccServiceStart();
      virtual void onFailedServiceStart(const ::scoped_string & reason);
      virtual void onServiceStop();

   //private:
      //WinEventLog m_sysLog;
      ::pointer < ::remoting_rtc::EventLog > m_peventlog;
   };
} // namespace remoting_node_desktop