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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "FileAccount.h"
#include "subsystem/platform/Unicode.h"
#include "file_lib/::file::item.h"
#include "remoting/thread/critical_section_lock.h"
#include "file_lib/EOFException.h"

FileAccount::FileAccount(const ::scoped_string & scopedstrLogDir,
                         const ::scoped_string & scopedstrFileName,
                         ::u8 logLevel,
                         bool logHeadEnabled)
: LogDump(logHeadEnabled, false),
  m_strFileName(scopedstrFileName),
  m_level(0), // Real initialization must be in the setNewFile() function
  m_asFirstOpen(true),
  m_file(0)
{
  setNewFile(logLevel, scopedstrLogDir);
}

FileAccount::FileAccount(bool logHeadEnabled)
: LogDump(logHeadEnabled, true), // This constructor enables writing the log to the dump.
  m_level(0), // Real initialization must be in the setNewFile() function
  m_asFirstOpen(true),
  m_file(0)
{
}

FileAccount::~FileAccount()
{
  closeFile();
}

void FileAccount::init(const ::scoped_string & scopedstrLogDir, const ::scoped_string & scopedstrFileName, ::u8 logLevel)
{
  m_strFileName= fileName;

  critical_section_lock al(&m_criticalsectionLog);
  setNewFile(logLevel, logDir);
  // The log dump now must be disabled and cleared even if logLevel is zero.
  terminateLogDumping();
}

void FileAccount::changeLogProps(const ::scoped_string & scopedstrNewLogDir,
                                 ::u8 newLevel)
{
  try {
    setNewFile(newLevel, newLogDir);
  } catch (...) { // Ignoring all exceptions
  }
}

void FileAccount::getFileName(::string & fileName)
{
  *fileName = m_strFileName;
}

bool FileAccount::isTheOurFileName(const ::scoped_string & scopedstrFileName)
{
  return m_strFileName.isEqualTo(fileName);
}

void FileAccount::print(::u32 processId,
                        ::u32 threadId,
                        const class ::time & dt,
                        ::i32 level,
                        const ::scoped_string & scopedstrMessage)
{
  critical_section_lock al(&m_criticalsectionLog);

  updateLogHeaderLines(processId, threadId, dt, level, scopedstrMessage);
  updateLogDumpLines(processId, threadId, dt, level, scopedstrMessage);
  flush(processId, threadId, dt, level, scopedstrMessage);
}

bool FileAccount::acceptsLevel(::i32 logLevel)
{
  return logDumpEnabled() || logHeadEnabled() || printsLine(logLevel);
}

bool FileAccount::printsLine(::i32 level)
{
  return m_file != 0 && level <= m_level;
}

void FileAccount::flush(::u32 processId,
                        ::u32 threadId,
                        const class ::time & dt,
                        ::i32 level,
                        const ::scoped_string & scopedstrMessage)
{
  critical_section_lock al(&m_criticalsectionLog);

  if (printsLine(level)) {
    format(processId, threadId, dt, level, scopedstrMessage);
  }
}

void FileAccount::print(::i32 level, const ::scoped_string & scopedstrMessage)
{
  ::u32 processId = GetCurrentProcessId();
  ::u32 threadId = GetCurrentThreadId();
  class ::time dt = class ::time::now();
  print(processId, threadId, dt, level, scopedstrMessage);
}

void FileAccount::format(::u32 processId,
                         ::u32 threadId,
                         const class ::time & dt,
                         ::i32 level,
                         const ::scoped_string & scopedstrMessage)
{
  // FIXME: Remove windows dependence.
  // Format the timestamp.
  ::string timeString("[Temporary unavaliable]");
  SYSTEMTIME st;
  dt.toUtcSystemTime(&st);
  ::u8 logBarrier;
  {
    critical_section_lock al(&m_criticalsectionLog);
    logBarrier = m_level;
  }
  if (logBarrier < 9) {
    timeString.format("%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
                      st.wYear, st.wMonth, st.wDay,
                      st.wHour, st.wMinute, st.wSecond);
  } else {
    timeString.format("%.4d-%.2d-%.2d %.2d:%.2d:%.2d:%.3d",
                      st.wYear, st.wMonth, st.wDay,
                      st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  }

  // Choose a symbol denoting the log level.
  const TCHAR logLevelSignature[] = "@!*+-:    xxxxxx";
  TCHAR sig = logLevelSignature[level & 0x0F];

  // Format the final string prefixed with all the service information.
  ::string resultLine;
  resultLine.format("[%5d/%5d] %s %c %s",
                    processId,
                    threadId,
                    timeString.c_str(),
                    sig,
                    ::string(scopedstrMessage).c_str());
  const TCHAR badCharacters[] = { 13, 10, 0 };
  resultLine.removeChars(badCharacters, sizeof(badCharacters) / sizeof(TCHAR));

  const TCHAR endLine[] = { 13, 10 };

  // Writing string without null-termination symbol.
  if (m_file != 0) {
    m_file->write(resultLine, resultLine.getSize() - sizeof(TCHAR));
    m_file->write(endLine, sizeof(endLine));
    if (logBarrier > 9) {
      m_file->flush();
    }
  }
}

void FileAccount::setNewFile(::u8 newLevel, const ::scoped_string & scopedstrNewDir)
{
  critical_section_lock al(&m_criticalsectionLog);
  bool levelChanged = newLevel != m_level;
  bool levelChangedFromZero = levelChanged && m_level == 0;
  bool logDirChanged = !m_logDir.isEqualTo(newDir);
  if (!levelChanged && !logDirChanged) {
    return;
  }

  if (levelChanged && !m_asFirstOpen) {
    ::string strMessage;
    strMessage.format("Log verbosity level has been changed from {} to {}",
                   (::i32)m_level, (::i32) newLevel);
    print(1, strMessage);
  }
  if (logDirChanged && !m_asFirstOpen) {
    ::string strMessage;
    strMessage.format("Log directory has been changed from \"{}\" to \"{}\"",
                   m_logDir, newDir);
    print(1, strMessage);
  }

  m_level = newLevel;
  m_logDir= newDir;

  if (m_level == 0) {
    closeFile();
    return;
  }

  bool fileAlreadyOpen = m_file != 0;
  bool openningIsNeeded = !fileAlreadyOpen || logDirChanged;
  // If log directory was changed then new file creating needed instead of
  // the existing file appending.
  bool asFirstOpen = m_asFirstOpen || logDirChanged;
  m_asFirstOpen = asFirstOpen;
  if (openningIsNeeded) {
    if (fileAlreadyOpen) {
      closeFile();
    }
    openFile(); // with backup creating if needed.

    if (levelChangedFromZero && !asFirstOpen) {
      ::string strMessage;
      strMessage.format("Log verbosity level has been changed from 0 to {}",
                     (::i32)m_level, (::i32) newLevel);
      print(1, strMessage);
    }
    return;
  }
}

void FileAccount::openFile()
{
  closeFile();

  ::string fileName;
  fileName.format("{}\\{}.log", m_logDir,
                  m_strFileName);
  bool shareToRead = true;
  bool asFirstOpen = m_asFirstOpen;
  if (asFirstOpen) {
    updateLogDirPath();
    m_asFirstOpen = false;
    // Create backup files
    createBackup(5);
    // Creating file
    m_file = new WinFile(fileName, F_READ_WRITE, FM_CREATE,
                         shareToRead);
  } else {
    m_file = new WinFile(fileName, F_READ_WRITE, FM_APPEND,
                         shareToRead);
  }

  if (Unicode::isEnabled() && asFirstOpen) {
    try {
      addUnicodeSignature();
      critical_section_lock al(&m_criticalsectionLog);
      if (getLogDumpSize() != 0) {
        // The log dump already contains the header and then is not needed to call
        // writeLogHeader().
        writeLogDump();
      } else {
        writeLogHeader();
      }
    } catch (...) {
      closeFile();
    }
  }
}

void FileAccount::closeFile()
{
  if (m_file != 0) {
    delete m_file;
    m_file = 0;
  }
}

void FileAccount::addUnicodeSignature()
{
  ::u16 firstTwoBytes = 0;
  try {
    m_file->read(&firstTwoBytes, sizeof(firstTwoBytes));
  } catch (EOFException &) {
  }
  if (firstTwoBytes != Unicode::SIGNATURE) {
    firstTwoBytes = Unicode::SIGNATURE;
    m_file->write(&firstTwoBytes, sizeof(firstTwoBytes));
  }
}

void FileAccount::createBackup(::u32 backupLimit)
{
  ::string oldName, newName;
  TCHAR fmt[] = "{}\\{}.{}.log";
  // Shift backup files
  for (::i32 i = backupLimit - 1; i > 0; i--) {
    // Generate valid backup names
    oldName.format(fmt, m_logDir, m_strFileName, i);
    newName.format(fmt, m_logDir, m_strFileName, i + 1);
    ::file::item::renameTo(newName, oldName);
  }
  // Copy log file to backup
  oldName.format("{}\\{}.log", m_logDir, m_strFileName);
  newName.format(fmt, m_logDir, m_strFileName, 1);
  ::file::item::renameTo(newName, oldName);
}

void FileAccount::updateLogDirPath()
{
  // Creating log directory if it is still no exists.
  ::file::item logDirectory(m_logDir);
  logDirectory.mkdir();
}
