// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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
#include "DesktopServerWatcher.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/platform/Exception.h"
#include "remoting/remoting_rfb/node_config/Configurator.h"
#include "acme/operating_system/shared_memory.h"
#include "subsystem/node/File.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem/platform/subsystem.h"
#include "subsystem/node/AnonymousPipeFactory.h"
#include "subsystem/node/CurrentConsoleProcess.h"
#ifdef WINDOWS
#include "subsystem_windows/node/CurrentConsoleProcess.h"
#include "subsystem_windows/node/AnonymousPipeFactory.h"
#include "subsystem_windows/node/EmulatedAnonymousPipeFactory.h"
//#include "subsystem_windows/node/SharedMemory.h"
#include "subsystem_windows/node/WTS.h"
#include "subsystem_windows/platform/subsystem.h"
#include "subsystem_windows/node/WinStaLibrary.h"
#include "subsystem_windows/node/WinHandles.h"
#include "subsystem_windows/node/SharedMemory.h"
#endif
//#include aaa_<time.h>

namespace remoting_rfb
{


   // DesktopServerWatcher::DesktopServerWatcher(ReconnectionListener *preconnectionlistener, ::subsystem::LogWriter * plogwriter) :
   //     m_preconnectionlistener(preconnectionlistener), m_pprocess(0), m_plogwriter = plogwriter;
   // {
   //    // Desktop server folder.
   //    ::string currentModulePath;
   //    currentModulePath = MainSubsystem().OperatingSystem().getCurrentModulePath();;
   //
   //    // Path to desktop server application.
   //    ::string path;
   //    // FIXME: To think: is quotes needed?
   //    path.format("\"{}\"", currentModulePath);
   //
   //    try
   //    {
   //       bool connectRdpSession = m_pconfigurator->getServerConfig()->getConnectToRdpFlag();
   //       m_pprocess = new CurrentConsoleProcess(m_plogwriter, connectRdpSession, path);
   //    }
   //    catch (...)
   //    {
   //       if (m_pprocess)
   //          delete m_pprocess;
   //       throw;
   //    }
   // }

   DesktopServerWatcher::DesktopServerWatcher() :
   Thread("DskSvrWatch"),m_preconnectionlistener(nullptr)
   {


   }

   DesktopServerWatcher::~DesktopServerWatcher()
   {
      //setThreadToFinish();
      //waitThreadToFinish();
      //delete m_pprocess;
   }

void DesktopServerWatcher::destroy()
{
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
   delete m_pprocess;
}


   void DesktopServerWatcher::initialize_desktop_server_watcher(::remoting_rfb_node::Configurator * pconfigurator, ReconnectionListener *preconnectionlistener, ::subsystem::LogWriter * plogwriter)
   {

      m_pconfigurator = pconfigurator;
      m_preconnectionlistener =preconnectionlistener;
      m_plogwriter = plogwriter;
      // Desktop server folder.
      ::string currentModulePath;
      currentModulePath = MainSubsystem().OperatingSystem().getCurrentModulePath();

      // Path to desktop server application.
      ::string path;
      // FIXME: To think: is quotes needed?
      path.format("\"{}\"", currentModulePath);

      try
      {
         bool bConnectRdpSession = m_pconfigurator->getServerConfig()->getConnectToRdpFlag();
         m_pprocess = MainSubsystem().createCurrentConsoleProcess(m_plogwriter, bConnectRdpSession, path);
      }
      catch (...)
      {
         if (m_pprocess)
            delete m_pprocess;
         throw;
      }
   }

   void DesktopServerWatcher::onThreadMain()
   {
      ::subsystem::AnonymousPipeFactory pipeFactory;

      pipeFactory.initialize_anonymous_pipe_factory(512 * 1024, m_plogwriter);

      ::pointer < ::subsystem::AnonymousPipeInterface > ownSidePipeChanTo, otherSidePipeChanTo, ownSidePipeChanFrom, otherSidePipeChanFrom;

      while (!isThreadTerminating())
      {
         try
         {
            ::string shMemName("Global\\");
            srand((::u32)time(0));
            for (::i32 i = 0; i < 20; i++)
            {
               shMemName+=(::i8)('a' + rand() % ('z' - 'a'));
            }

            auto psharedmemory = createø<::shared_memory>();

            psharedmemory->CreateSharedMemory(shMemName, 72);

            ::u64 *mem = (::u64 *)psharedmemory->data();

            // Sets memory ready flag to false.
            mem[0] = 0;

            ownSidePipeChanTo = otherSidePipeChanTo = ownSidePipeChanFrom = otherSidePipeChanFrom = 0;

            pipeFactory.generatePipes(ownSidePipeChanTo, false, otherSidePipeChanTo, false);
            pipeFactory.generatePipes(ownSidePipeChanFrom, false, otherSidePipeChanFrom, false);

            // Remoting Node log directory.
            ::string logDir;
            m_pconfigurator->getServerConfig()->getLogFileDir(logDir);

            // Arguments that must be passed to desktop server application.
            ::string args;
            args.format("-desktopserver -logdir \"{}\" -loglevel {} -shmemname {}", logDir,
                         m_pconfigurator->getServerConfig()->getLogLevel(), shMemName);

            m_pprocess->setArguments(args);
            start();

            // Prepare other side pipe handles for other side
            m_plogwriter->debug("DesktopServerWatcher::onThreadMain(): assigning handles");
            otherSidePipeChanTo->assignHandlesFor(m_pprocess->getProcessHandle(), false);
            otherSidePipeChanFrom->assignHandlesFor(m_pprocess->getProcessHandle(), false);

             ::cast <subsystem::PipeInterface > ppipeTo = otherSidePipeChanTo;
             ::cast <subsystem::PipeInterface > ppipeFrom = otherSidePipeChanFrom;

            // Transfer other side handles by the memory channel
            mem[1] = (::u64)otherSidePipeChanTo->getWriteFile()->_HANDLE();
            mem[2] = (::u64)otherSidePipeChanTo->getReadFile()->_HANDLE();
            mem[3] = (::u64)ppipeTo->getMaxPortionSize();
            mem[4] = (::u64)otherSidePipeChanFrom->getWriteFile()->_HANDLE();
            mem[5] = (::u64)otherSidePipeChanFrom->getReadFile()->_HANDLE();
            mem[6] = (::u64)ppipeFrom->getMaxPortionSize();

            // Sets memory ready flag to true.
            mem[0] = 1;

            // Destroying other side objects
            delete otherSidePipeChanTo;
            m_plogwriter->debug("DesktopServerWatcher::onThreadMain(): Destroyed otherSidePipeChanTo");
            otherSidePipeChanTo = 0;
            delete otherSidePipeChanFrom;
            m_plogwriter->debug("DesktopServerWatcher::onThreadMain(): Destroyed otherSidePipeChanFrom");
            otherSidePipeChanFrom = 0;

            m_plogwriter->debug("DesktopServerWatcher::onThreadMain(): Try to call onReconnect()");
            m_preconnectionlistener->onReconnect(ownSidePipeChanTo, ownSidePipeChanFrom);

            m_pprocess->waitForExit();
         }
         catch (::exception &e)
         {
            // A potentional memory leak.
            // A potential crash. The channels can be used (see onReconnect()) after these destroyings.
            if (ownSidePipeChanTo)
               delete ownSidePipeChanTo;
            if (otherSidePipeChanTo)
               delete otherSidePipeChanTo;
            if (ownSidePipeChanFrom)
               delete ownSidePipeChanFrom;
            if (otherSidePipeChanFrom)
               delete otherSidePipeChanFrom;
            m_plogwriter->error("DesktopServerWatcher has failed with error: {}", e.get_message());
            preempt(1_s);
         }
      }
   }

   void DesktopServerWatcher::onTermThread() { m_pprocess->stopWait(); }

   void DesktopServerWatcher::start()
   {
      ::i32 pipeNotConnectedErrorCount = 0;

      for (::i32 i = 0; i < 5; i++)
      {
         try
         {
            m_pprocess->start();
            return;
         }
         catch (::subsystem::SystemException &sysEx)
         {
            // // It can be XP specific error.
            // if (sysEx.getErrorCode() == 233 || sysEx.getErrorCode() == 87)
            // {
            //    pipeNotConnectedErrorCount++;
            //
            //    DWORD sessionId = WindowsSubsystem().WTS().getActiveConsoleSessionId(m_plogwriter);
            //
            //    bool isXPFamily = MainSubsystem().OperatingSystem().isWinXP() || MainSubsystem().OperatingSystem().isWin2003Server();
            //    bool needXPTrick = (isXPFamily) && (sessionId > 0) && (pipeNotConnectedErrorCount >= 3);
            //
            //    // Try start as current user with xp trick.
            //    if (needXPTrick)
            //    {
            //       doXPTrick();
            //       m_pprocess->start();
            //       return;
            //    }
            // }
            // else
            {
               throw;
            }
         }
         preempt(3_s);
      } // for
   }

   // void DesktopServerWatcher::doXPTrick()
   // {
   //    m_plogwriter->information("Trying to do WindowsXP trick to start process on separate session");
   //
   //    try
   //    {
   //       WinStaLibrary winSta;
   //
   //       WCHAR password[1];
   //       memset(password, 0, sizeof(password));
   //
   //       if (winSta.WinStationConnectW(NULL, 0, WindowsSubsystem().WTS().getActiveConsoleSessionId(m_plogwriter), password, 0) == false)
   //       {
   //          throw SystemException("Failed to call WinStationConnectW");
   //       }
   //
   //       // Get path to remoting_rfb_node binary.
   //       ::string pathToBinary;
   //       pathToBinary = MainSubsystem().OperatingSystem().getCurrentModulePath();;
   //
   //       // Start current console process that will lock workstation (not using Xp Trick).
   //       CurrentConsoleProcess lockWorkstation(m_plogwriter, false, pathToBinary, "-lockworkstation");
   //       lockWorkstation.start();
   //       lockWorkstation.waitForExit();
   //
   //       // Check exit code (exit code is GetLastError() value in case of system error,
   //       // LockWorkstation() in child process failed, or 0 if workstation is locked).
   //       DWORD exitCode = lockWorkstation.getExitCode();
   //
   //       if (exitCode != 0)
   //       {
   //          throw SystemException(exitCode);
   //       }
   //    }
   //    catch (SystemException &ex)
   //    {
   //       m_plogwriter->error(ex.get_message());
   //       throw;
   //    }
   // }


} // namespace remoting_rfb




