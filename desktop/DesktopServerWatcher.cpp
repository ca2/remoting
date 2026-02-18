// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopServerWatcher.h"
//#include "win_system/Environment.h"
#include "util/Exception.h"
#include "server_config_lib/Configurator.h"
#include "win_system/CurrentConsoleProcess.h"
#include "win_system/AnonymousPipeFactory.h"
#include "win_system/EmulatedAnonymousPipeFactory.h"
#include "win_system/WTS.h"
#include "win_system/WinStaLibrary.h"
#include "win_system/WinHandles.h"
#include "win_system/SharedMemory.h"

#include <time.h>

DesktopServerWatcher::DesktopServerWatcher(ReconnectionListener *recListener, LogWriter *log)
: m_recListener(recListener),
  m_process(0),
  m_log(log)
{
  // Desktop server folder.
  ::string currentModulePath;
  Environment::getCurrentModulePath(&currentModulePath);

  // Path to desktop server application.
  ::string path;
  // FIXME: To think: is quotes needed?
  path.formatf("\"{}\"", currentModulePath);

  try {
    bool connectRdpSession = Configurator::getInstance()->getServerConfig()->getConnectToRdpFlag();
    m_process = new CurrentConsoleProcess(m_log, connectRdpSession, path);
  } catch (...) {
    if (m_process) delete m_process;
    throw;
  }
}

DesktopServerWatcher::~DesktopServerWatcher()
{
  terminate();
  wait();
  delete m_process;
}

void DesktopServerWatcher::execute()
{
  AnonymousPipeFactory pipeFactory(512 * 1024, m_log);

  AnonymousPipe *ownSidePipeChanTo, *otherSidePipeChanTo,
                *ownSidePipeChanFrom, *otherSidePipeChanFrom;

  while (!isTerminating()) {
    try {
      ::string shMemName("Global\\");
      srand((unsigned)time(0));
      for (int i = 0; i < 20; i++) {
        shMemName.appendChar('a' + rand() % ('z' - 'a'));
      }
      SharedMemory sharedMemory(shMemName, 72);
      unsigned long long *mem = (unsigned long long *)sharedMemory.getMemPointer();

      // Sets memory ready flag to false.
      mem[0] = 0;

      ownSidePipeChanTo = otherSidePipeChanTo =
      ownSidePipeChanFrom = otherSidePipeChanFrom = 0;

      pipeFactory.generatePipes(&ownSidePipeChanTo, false,
                                &otherSidePipeChanTo, false);
      pipeFactory.generatePipes(&ownSidePipeChanFrom, false,
                                &otherSidePipeChanFrom, false);

      // TightVNC server log directory.
      ::string logDir;
      Configurator::getInstance()->getServerConfig()->getLogFileDir(&logDir);

      // Arguments that must be passed to desktop server application.
      ::string args;
      args.formatf("-desktopserver -logdir \"{}\" -loglevel {} -shmemname {}",
                  logDir,
                  Configurator::getInstance()->getServerConfig()->getLogLevel(),
                  shMemName);

      m_process->setArguments(args);
      start();

      // Prepare other side pipe handles for other side
      m_log->debug("DesktopServerWatcher::execute(): assigning handles");
      otherSidePipeChanTo->assignHandlesFor(m_process->getProcessHandle(), false);
      otherSidePipeChanFrom->assignHandlesFor(m_process->getProcessHandle(), false);

      // Transfer other side handles by the memory channel
      mem[1] = (unsigned long long)otherSidePipeChanTo->getWriteHandle();
      mem[2] = (unsigned long long)otherSidePipeChanTo->getReadHandle();
      mem[3] = (unsigned long long)otherSidePipeChanTo->getMaxPortionSize();
      mem[4] = (unsigned long long)otherSidePipeChanFrom->getWriteHandle();
      mem[5] = (unsigned long long)otherSidePipeChanFrom->getReadHandle();
      mem[6] = (unsigned long long)otherSidePipeChanFrom->getMaxPortionSize();

      // Sets memory ready flag to true.
      mem[0] = 1;

      // Destroying other side objects
      delete otherSidePipeChanTo;
      m_log->debug("DesktopServerWatcher::execute(): Destroyed otherSidePipeChanTo");
      otherSidePipeChanTo = 0;
      delete otherSidePipeChanFrom;
      m_log->debug("DesktopServerWatcher::execute(): Destroyed otherSidePipeChanFrom");
      otherSidePipeChanFrom = 0;

      m_log->debug("DesktopServerWatcher::execute(): Try to call onReconnect()");
      m_recListener->onReconnect(ownSidePipeChanTo, ownSidePipeChanFrom);

      m_process->waitForExit();

    } catch (::exception &e) {
      // A potentional memory leak. 
      // A potential crash. The channels can be used (see onReconnect()) after these destroyings.
      if (ownSidePipeChanTo) delete ownSidePipeChanTo;
      if (otherSidePipeChanTo) delete otherSidePipeChanTo;
      if (ownSidePipeChanFrom) delete ownSidePipeChanFrom;
      if (otherSidePipeChanFrom) delete otherSidePipeChanFrom;
      m_log->error("DesktopServerWatcher has failed with error: {}", e.get_message());
      Sleep(1000);
    }
  }
}

void DesktopServerWatcher::onTerminate()
{
  m_process->stopWait();
}

void DesktopServerWatcher::start()
{
  int pipeNotConnectedErrorCount = 0;

  for (int i = 0; i < 5; i++) {
    try {
      m_process->start();
      return;
    } catch (SystemException &sysEx) {
      // It can be XP specific error.
      if (sysEx.getErrorCode() == 233 || sysEx.getErrorCode() == 87) {
        pipeNotConnectedErrorCount++;

        DWORD sessionId = WTS::getActiveConsoleSessionId(m_log);

        bool isXPFamily = Environment::isWinXP() || Environment::isWin2003Server();
        bool needXPTrick = (isXPFamily) && (sessionId > 0) && (pipeNotConnectedErrorCount >= 3);

        // Try start as current user with xp trick.
        if (needXPTrick) {
          doXPTrick();
          m_process->start();
          return;
        }
      } else {
        throw;
      }
    }
    Sleep(3000);
  } // for 
}

void DesktopServerWatcher::doXPTrick()
{
  m_log->information("Trying to do WindowsXP trick to start process on separate session");

  try {
    WinStaLibrary winSta;

    WCHAR password[1];
    memset(password, 0, sizeof(password));

    if (winSta.WinStationConnectW(NULL, 0, WTS::getActiveConsoleSessionId(m_log),
      password, 0) == FALSE) {
      throw SystemException("Failed to call WinStationConnectW");
    }

    // Get path to remoting_node binary.
    ::string pathToBinary;
    Environment::getCurrentModulePath(&pathToBinary);

     // Start current console process that will lock workstation (not using Xp Trick).
    CurrentConsoleProcess lockWorkstation(m_log, false, pathToBinary,
      "-lockworkstation");
    lockWorkstation.start();
    lockWorkstation.waitForExit();

     // Check exit code (exit code is GetLastError() value in case of system error,
     // LockWorkstation() in child process failed, or 0 if workstation is locked).
    DWORD exitCode = lockWorkstation.getExitCode();

    if (exitCode != 0) {
      throw SystemException(exitCode);
    }
  } catch (SystemException &ex) {
    m_log->error(ex.get_message());
    throw;
  }
}
