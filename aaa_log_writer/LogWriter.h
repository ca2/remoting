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


// #include "LogWriter.h"
// #include "util/CharDefs.h"
// #include "ProfileLogWriter.h"

// This class is a high level wrap for the LogWriter class. It helps write log in different
// log levels by the different functions.
class LogWriter :
virtual public ::particle
{
public:
//   // The LogWriter argument is a pointer to a LogWriter object that implements log writing.
//   // It can be zero, in this case no log is writing.
//   LogWriter(LogWriter *LogWriter);
//   virtual ~LogWriter();
//
//   // Write most important log messages such as code internal errors
//   // (what shouldn't have happened but it did).
//   void interror(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write other important log messages that do not give continue work normally, such
//   // as connection errors and other.
//   void error(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write logs with the warning level that less that error level.
//   void warning(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write logs with neutral log levels.
//   void scopedstrMessage(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write logs with info level which less than the neutral "scopedstrMessage" level.
//   void info(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write logs with detailed level when need to get more details.
//   void detail(const ::scoped_string & scopedstrFmt, ...);
//
//   // Write logs only for debug purposes, it most of detailed logs. Should to be used
//   // to write log messages which helps to track work of a code as detailed as possible.
//   void debug(const ::scoped_string & scopedstrFmt, ...);
//
//   // Returnd true if debug loglevel enabled.
//   bool isDebug();
//
//   // Add profiler checkpoint
//   ProcessorTimes checkPoint(const ::scoped_string & scopedstrTag);
//
// protected:
//   static const int LOG_INTERR = 0;
//   static const int LOG_ERR = 1;
//   static const int LOG_WARN = 2;
//   static const int LOG_MSG = 3;
//   static const int LOG_INFO = 4;
//   static const int LOG_DETAIL = 5;
//   static const int LOG_DEBUG = 9;
//
// private:
//   void vprintLog(int logLevel, const ::scoped_string & scopedstrFmt, va_list argList);
//
//   LogWriter *m_LogWriter;
//   ProfileLogWriter *m_profiler;
};

////// _LOGWRITER_H_
