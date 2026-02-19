// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "WinEventLogWriter.h"
#include "win-event-log/MessageProvider.h"

WinEventLogWriter::WinEventLogWriter(LogWriter *log)
: m_sysLog(log)
{
}

WinEventLogWriter::~WinEventLogWriter()
{
}

void WinEventLogWriter::enable()
{
  m_sysLog.enable();
}

void WinEventLogWriter::onSuccAuth(const ::scoped_string & ip)
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE,
                      "Authentication passed by {}",
                      ip->getString());
}

void WinEventLogWriter::onAuthFailed(const ::scoped_string & ip)
{
  m_sysLog.reportWarning(MSG_WARNING_MESSAGE,
                         "Authentication failed from {}",
                         ip->getString());
}

void WinEventLogWriter::onDisconnect(const ::scoped_string & scopedstrMessage)
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE, "{}", scopedstrMessage->getString());
}

void WinEventLogWriter::onCrash(const ::scoped_string & dumpPath)
{
  m_sysLog.reportError(MSG_ERROR_MESSAGE,
                       "Application crashed. Debug information has been saved to {}",
                       dumpPath->getString());
}

void WinEventLogWriter::onSuccServiceStart()
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE,
                      "Service has been started successfully");
}

void WinEventLogWriter::onFailedServiceStart(const ::scoped_string & reason)
{
  m_sysLog.reportError(MSG_ERROR_MESSAGE,
                       "Service has been terminated for the following reason: {}",
                       reason->getString());
}

void WinEventLogWriter::onServiceStop()
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE, "Service has been stopped");
}
