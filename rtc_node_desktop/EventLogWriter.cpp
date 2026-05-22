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
#include "framework.h"
#include "EventLogWriter.h"
#include "remoting/remoting_rtc/event_log/EventLog.h"
//#include "win-event-log/MessageProvider.h"
namespace remoting_node_desktop
{
   EventLogWriter::EventLogWriter(::subsystem::LogWriter * plogwriter)
   //: m_sysLog(plogwriter)
   {
      m_peventlog = allocateø ::remoting_rtc::EventLog();
      m_peventlog->m_plogwriter = plogwriter;
   }

   EventLogWriter::~EventLogWriter()
   {
   }

   void EventLogWriter::enable()
   {
      m_peventlog->enable();
   }

   void EventLogWriter::onSuccAuth(const ::scoped_string & ip)
   {
      m_peventlog->reportInfo(0, // MSG_INFO_MESSAGE,
                          "Authentication passed by {}",
                          ::string(ip).c_str());
   }

   void EventLogWriter::onAuthFailed(const ::scoped_string & ip)
   {
      m_peventlog->reportWarning(0, // MSG_WARNING_MESSAGE,
                             "Authentication failed from {}",
                             ::string(ip).c_str());
   }

   void EventLogWriter::onDisconnect(const ::scoped_string & scopedstrMessage)
   {
      m_peventlog->reportInfo(0, // MSG_INFO_MESSAGE,
         "{}", ::string(scopedstrMessage).c_str());
   }

   void EventLogWriter::onCrash(const ::scoped_string & dumpPath)
   {
      m_peventlog->reportError(0, // MSG_ERROR_MESSAGE,
                           "Application crashed. Debug information has been saved to %s",
                           ::string(dumpPath).c_str());
   }

   void EventLogWriter::onSuccServiceStart()
   {
      m_peventlog->reportInfo(0, // MSG_INFO_MESSAGE,
                          "Service has been started successfully");
   }

   void EventLogWriter::onFailedServiceStart(const ::scoped_string & reason)
   {
      m_peventlog->reportError(0, // MSG_ERROR_MESSAGE,
                           "Service has been terminated for the following reason: {}",
                           ::string(reason).c_str());
   }

   void EventLogWriter::onServiceStop()
   {
      m_peventlog->reportInfo(0, // MSG_INFO_MESSAGE,
         "Service has been stopped");
   }
} // namespace remoting_node_desktop

