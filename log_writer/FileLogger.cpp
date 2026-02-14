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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "FileLogger.h"

FileLogger::FileLogger(const ::scoped_string & scopedstrlogDir, const ::scoped_string & scopedstrFileName,
                       unsigned char logLevel, bool logHeadEnabled)
: m_fileAccount(logDir, fileName, logLevel, logHeadEnabled)
{
}

FileLogger::FileLogger(bool logHeadEnabled)
: m_fileAccount(logHeadEnabled)
{
}

FileLogger::~FileLogger()
{
}

void FileLogger::init(const ::scoped_string & scopedstrlogDir, const ::scoped_string & scopedstrFileName, unsigned char logLevel)
{
  m_fileAccount.init(logDir, fileName, logLevel);
}

void FileLogger::storeHeader()
{
  m_fileAccount.storeHeader();
}

void FileLogger::print(int logLevel, const ::scoped_string & scopedstrline)
{
  try {
    unsigned int processId = GetCurrentProcessId();
    unsigned int threadId = GetCurrentThreadId();
    DateTime currTime = DateTime::now();

    m_fileAccount.print(processId, threadId, currTime, logLevel, line);
  } catch (...) {
  }
}

bool FileLogger::acceptsLevel(int logLevel)
{
  return m_fileAccount.acceptsLevel(logLevel);
}

void FileLogger::changeLogProps(const ::scoped_string & scopedstrNewLogDir, unsigned char newLevel)
{
  m_fileAccount.changeLogProps(newLogDir, newLevel);
}
