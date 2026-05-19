// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
#include "subsystem/_common_header.h"
////#include "remoting/node_desktop/NamingDefs.h"
#include "HooksUpdateDetector.h"

#include "acme/operating_system/windows/_.h"
#include "subsystem_windows/node/MessageWindow.h"
#include "subsystem_windows/node/UipiControl.h"
#include "subsystem/node/OperatingSystem.h"
#include "subsystem_windows/node/UipiControl.h"
#include "subsystem/node/OperatingSystem.h"

namespace remoting_windows
{

//    HooksUpdateDetector::HooksUpdateDetector(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener,
//                                             ::subsystem::LogWriter * plogwriter) :
//        UpdateDetector(pupdatekeeper, pupdatelistener), m_phookupdatetimer(pupdatelistener), m_pmessagewindowTarget(0), m_hookInstaller(0),
//        m_plogwriter = plogwriter;
//    {
// #ifndef _WIN64
//       m_plogwriter->debug("Loading the screenhook library for 32bit system");
// #else
//       m_plogwriter->debug("Loading the screenhook library for 64bit system");
// #endif
//       try
//       {
//          m_hookInstaller = new HookInstaller();
//       }
//       catch (::exception &e)
//       {
//          Thread::setThreadToFinish();
//          m_plogwriter->error("Failed to load the hook library: {}", e.get_message());
//       }
//       //HINSTANCE hinst = GetModuleHandle(0);
//       auto hinst = (HINSTANCE) windows::hinstance_from_function(::windows::window::s_window_procedure);
//       m_pmessagewindowTarget = new MessageWindow(hinst, "HookTargetWinClassName");
//    }

   HooksUpdateDetector::HooksUpdateDetector():
   m_pmessagewindowTarget(0),m_hookInstaller(0)
   {
   }

   HooksUpdateDetector::~HooksUpdateDetector()
   {
      setThreadToFinish();
      waitThreadToFinish();

      if (m_hookInstaller != 0)
      {
         delete m_hookInstaller;
      }
      if (m_pmessagewindowTarget != 0)
      {
         delete m_pmessagewindowTarget;
      }
   }


   void HooksUpdateDetector::initialize_hooks_update_detector(::remoting::UpdateKeeper * pupdatekeeper, ::remoting::UpdateListener * pupdatelistener, ::subsystem::LogWriter * plogwriter)
   {
      initialize_update_detector(pupdatekeeper, pupdatelistener);
      construct_newø(m_phookupdatetimer);
      m_phookupdatetimer->initialize_hook_update_timer(pupdatelistener);
      //m_pmessagewindowTarget(0), m_hookInstaller(0),
   m_plogwriter = plogwriter;
   //{
#ifndef _WIN64
      m_plogwriter->debug("Loading the screenhook library for 32bit system");
#else
      m_plogwriter->debug("Loading the screenhook library for 64bit system");
#endif
      try
      {
         m_hookInstaller = new HookInstaller();
      }
      catch (::exception &e)
      {
         Thread::setThreadToFinish();
         m_plogwriter->error("Failed to load the hook library: {}", e.get_message());
      }
      //HINSTANCE hinst = GetModuleHandle(0);
      //auto hinst = (HINSTANCE) windows::hinstance_from_function(::windows::window::s_window_procedure);
      //m_pmessagewindowTarget = new MessageWindow(hinst, "HookTargetWinClassName");
      construct_newø(m_pmessagewindowTarget);
      m_pmessagewindowTarget->createMessageWindow("HookTargetWinClassName");
   }

   void HooksUpdateDetector::onTermThread()
   {
      if (m_pmessagewindowTarget != 0)
      {
         PostMessage((HWND)m_pmessagewindowTarget->_HWND(), WM_QUIT, 0, 0);
      }
      m_initWaiter.set_happening();
   }

   void HooksUpdateDetector::start32Loader()
   {
#ifdef _WIN64
      m_plogwriter->debug("Loading the screenhook library for 32bit system with hookldr.exe");
      if (!isThreadTerminating())
      {
         ::string path, folder;
         folder = MainSubsystem().OperatingSystem().getCurrentModuleFolderPath();;
         path.format("{}\\{}", folder, "hookdlr.exe");
         m_hookLoader32.setFilename(path);
         ::string hwndStr;
         hwndStr.format("%I64u", (DWORD64)m_pmessagewindowTarget->_HWND());
         m_hookLoader32.setArguments(hwndStr);
         try
         {
            m_hookLoader32.start();
         }
         catch (::exception &e)
         {
            m_plogwriter->error("Can't onThreadMain the 32-bit hook loader: {}", e.get_message());
         }
      }
#endif
   }

   void HooksUpdateDetector::terminate32Loader()
   {
      if (m_hookLoader32.getProcessHandle() != 0)
      {
         // Send broadcast scopedstrMessage to close the 32 bit hook loader.
         //broadcastMessage(HookDefinitions::LOADER_CLOSE_CODE);
         broadcastMessage(MainSubsystem().get_LOADER_CLOSE_CODE());
      }
   }

   void HooksUpdateDetector::broadcastMessage(unsigned int scopedstrMessage)
   {
      HWND hwndFound = FindWindowEx(HWND_MESSAGE, 0, 0, 0);
      while (hwndFound)
      {
         PostMessage(hwndFound, scopedstrMessage, 0, 0);
         hwndFound = GetNextWindow(hwndFound, GW_HWNDNEXT);
      }
   }

   void HooksUpdateDetector::onThreadMain()
   {
      m_plogwriter->information("Hooks update detector thread id = {}", getThreadId());

      if (!isThreadTerminating() && m_pmessagewindowTarget != 0)
      {
         m_pmessagewindowTarget->createMessageWindow();
         m_plogwriter->information("Hooks target window has been created (hwnd = {})", (::iptr)m_pmessagewindowTarget->_HWND());
      }

      try
      {
         ::subsystem_windows::UipiControl uipiControl(m_plogwriter);
         uipiControl.allowMessage(MainSubsystem().get_SPEC_IPC_CODE(), m_pmessagewindowTarget->_HWND());
      }
      catch (::exception &e)
      {
         setThreadToFinish();
         m_plogwriter->error(e.get_message());
      }

      bool hookInstalled = false;
      while (!isThreadTerminating() && !hookInstalled)
      {
         try
         {
            m_hookInstaller->install(m_pmessagewindowTarget->getHWND());
            hookInstalled = true;
         }
         catch (::exception &e)
         {
            m_plogwriter->error("Hooks installing failed, waitThreadToFinish for the next trying: {}", e.get_message());
            m_initWaiter.waitThreadToFinish(5000 * 1_ms);
            try
            {
               m_hookInstaller->uninstall();
            }
            catch (::exception &e)
            {
               m_plogwriter->error("Hooks uninstalling failed: {}", e.get_message());
            }
         }
      }

      start32Loader();

      if (!isThreadTerminating())
      {
         m_plogwriter->information("Hooks update detector has been successfully initialized");
      }

      MSG msg;
      while (!isThreadTerminating())
      {
         if (PeekMessage(&msg, m_pmessagewindowTarget->getHWND(), 0, 0, PM_REMOVE) != 0)
         {
            if (msg.scopedstrMessage == HookDefinitions::SPEC_IPC_CODE)
            {
               ::i32_rectangle rectangle((short)(msg.wParam >> 16), (short)(msg.wParam & 0xffff), (short)(msg.lParam >> 16),
                                    (short)(msg.lParam & 0xffff));
               if (rectangle.has_area())
               {
                  m_pupdatekeeper->addChangedRect(rectangle);
                  m_phookupdatetimer.sear();
               }
               //        m_plogwriter->debug("Screenhook update rectangle: {x={}, y={}, w={}, h={}}", rectangle.left,
               //        rectangle.top, rectangle.width(), rectangle.height());
            }
            else
            {
               DispatchMessage(&msg);
            }
         }
         else
         {
            if (WaitMessage() == 0)
            {
               m_plogwriter->error("Hooks update detector has failed");
               Thread::setThreadToFinish();
            }
         }
      }

      try
      {
         if (m_hookInstaller != 0)
         {
            m_hookInstaller->uninstall();
         }
         terminate32Loader();
      }
      catch (::exception &e)
      {
         m_plogwriter->error("{}", e.get_message());
      }
      m_plogwriter->information("Hooks update detector has been terminated.");
   }


} // namespace remoting_windows
 
