// Copyright (C) 2012 GlavSoft LLC.
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

//
// //#include "util/::earth::time.h"
//
// struct LogEntry
// {
//   LogEntry(unsigned int processId,
//            unsigned int threadId,
//            const ::earth::time & dt,
//            int lineLevel,
//            const ::scoped_string & scopedstrMessage);
//
//   unsigned int m_processId;
//   unsigned int m_threadId;
//   ::earth::time m_dt;
//   int m_lineLevel;
//   ::string m_message;
// };
//
// // This class is not thread safed.
// class LogDump
// {
// public:
//   LogDump(bool logHeadEnabled, bool logDumpEnabled);
//   virtual ~LogDump();
//
//   // Writes a log scopedstrMessage.
//   virtual void flush(unsigned int processId,
//                      unsigned int threadId,
//                      const ::earth::time & dt,
//                      int level,
//                      const ::scoped_string & scopedstrMessage) = 0;
//
//   // Stores all printed lines as a log header and stops it accumulation.
//   void storeHeader();
//
//   size_t getLogDumpSize();
//
// protected:
//   // This function checks that writing to header is not stopped and
//   // then store the log line information.
//   // This function must be calling under the log mutex from derived classes.
//   void updateLogHeaderLines(unsigned int processId,
//                             unsigned int threadId,
//                             const ::earth::time & dt,
//                             int level,
//                             const ::scoped_string & scopedstrMessage);
//
//   // This function checks that writing to the log dump is not stopped and
//   // then store the log line information.
//   // This function must be calling under the log mutex from derived classes.
//   void updateLogDumpLines(unsigned int processId,
//                           unsigned int threadId,
//                           const ::earth::time & dt,
//                           int level,
//                           const ::scoped_string & scopedstrMessage);
//
//   // Terminaties to write log lines to the memory buffer and clear the buffer.
//   void terminateLogDumping();
//
//   // This function write the log headers to a file if the log headers is ready
//   // (the storeHeader() function has been already called).
//   void writeLogHeader();
//
//   // This function writes the log dump if it is presented and then zeroes and disables it.
//   void writeLogDump();
//
//   // Returns true if log dumping is still enabled.
//   bool logDumpEnabled();
//
//   // Returns true if log head is still enabled.
//   bool logHeadEnabled();
//
// private:
//   static const size_t MAX_LOG_HEADER_SIZE = 16;
//   bool m_logHeaderStopped;
//   ::array_base<LogEntry> m_logHeaderLines;
//
//   static const size_t MAX_LOG_DUMP_SIZE = 1024;
//   bool m_logDumpStopped;
//   ::array_base<LogEntry> m_logDumpLines;
// };
//
// //// __LOGDUMP_H__
