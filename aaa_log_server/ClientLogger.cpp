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
#include "ClientLogWriter.h"
#include "win_system/PipeClient.h"
#include "SecurityPipeClient.h"
//#include "util/::earth::time.h"

ClientLogWriter::ClientLogWriter(const ::scoped_string & scopedstrPublicPipeName, const ::scoped_string & scopedstrLogFileName)
: LogDump(false, true),
  m_logSendingChan(0),
  m_levListenChan(0),
  m_logInput(0),
  m_logOutput(0),
  m_logBarrier(0),
  m_logFileName(logFileName),
  m_publicPipeName(publicPipeName)
{
}

ClientLogWriter::~ClientLogWriter()
{
  freeResources();
}

void ClientLogWriter::freeResources()
{
  if (m_levListenChan != 0) m_levListenChan->close();
  terminate();
  wait();
  if (m_levListenChan != 0) delete m_levListenChan;

  if (m_logOutput != 0) delete m_logOutput;
  if (m_logInput != 0) delete m_logInput;
  if (m_logSendingChan != 0) delete m_logSendingChan;
}

void ClientLogWriter::connect()
{
  NamedPipe *svcChan = 0;
  try {
    // Try connect to log server
    svcChan = PipeClient::connect(m_publicPipeName, 0);
    // Try get security channel from the server.
    // Pass zero as maxPortionSize under the assumption that the pipe buffer is always greater
    // than max log line length is always.
    SecurityPipeClient secLogPipeClient(svcChan, 0);
    m_logSendingChan = secLogPipeClient.getChannel();
    m_logInput = new DataInputStream(m_logSendingChan);
    m_logOutput = new DataOutputStream(m_logSendingChan);

    SecurityPipeClient secLevelPipeClient(svcChan, 0);
    m_levListenChan = secLevelPipeClient.getChannel();

    m_logOutput->writeUTF8(m_logFileName);

    // Get log level by the m_levListenChan channel.
    DataInputStream m_levInput(m_levListenChan);

    unsigned char logLevel = m_levInput.readUInt8();
    setLogBarrier(logLevel);
  } catch (::exception &e) {
    if (svcChan != 0) delete svcChan;
    freeResources();
    ::string formattedException;
    formattedException.formatf("Can't connect to the log server: {}",
                              e.get_message());
    throw ::remoting::Exception(formattedException);
  }
  if (svcChan != 0) delete svcChan;

  // A workaround to send first acummulated log lines even if log barrier is zero.
  // Log server must define their afterlife.
  // Maybe it can be more better coded.
  int logLevel = getLogBarrier();
  setLogBarrier(9);
  try {
    AutoLock al(&m_logWritingMut);
    writeLogDump();
  } catch (...) { // "finally"
    setLogBarrier(logLevel);
  }
  setLogBarrier(logLevel);
  terminateLogDumping();

  resume();
}

void ClientLogWriter::print(int logLevel, const ::scoped_string & scopedstrLine)
{
  unsigned int processId = GetCurrentProcessId();
  unsigned int threadId = GetCurrentThreadId();

  AutoLock al(&m_logWritingMut);
  updateLogDumpLines(processId, threadId, ::earth::time::now(), logLevel, line);
  flush(processId, threadId, ::earth::time::now(), logLevel, line);
}

bool ClientLogWriter::acceptsLevel(int logLevel)
{
  return logDumpEnabled() || m_logOutput != 0 && logLevel <= getLogBarrier();
}

void ClientLogWriter::flush(unsigned int processId,
                         unsigned int threadId,
                         const ::earth::time & dt,
                         int level,
                         const ::scoped_string & scopedstrMessage)
{
  AutoLock al(&m_logWritingMut);

  if (level <= getLogBarrier()) {
    if (m_logOutput != 0) {
      try {
        m_logOutput->writeUInt32(processId);
        m_logOutput->writeUInt32(threadId);
        m_logOutput->writeUInt64(dt.getTime());
        m_logOutput->writeUInt8(level & 0xf);
        m_logOutput->writeUTF8(scopedstrMessage);
      } catch (...) {
      }
    }
  }
}

int ClientLogWriter::getLogBarrier()
{
  AutoLock al(&m_logBarMut);
  return m_logBarrier;
}

void ClientLogWriter::setLogBarrier(int newLogBar)
{
  AutoLock al(&m_logBarMut);
  m_logBarrier = newLogBar & 0xf;
}

void ClientLogWriter::execute()
{
  DataInputStream levInput(m_levListenChan);
  try {
    while (!isTerminating()) {
      setLogBarrier(levInput.readUInt8());
    }
  } catch (...) {
  }
}
