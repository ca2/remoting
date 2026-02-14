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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "util/CommonHeader.h"
#include "remoting_node_desktop/NamingDefs.h"
#include "HooksUpdateDetector.h"

#include "win_system/UipiControl.h"
#include "win_system/Environment.h"

HooksUpdateDetector::HooksUpdateDetector(UpdateKeeper *updateKeeper,
                                         UpdateListener *updateListener, LogWriter *log)
: UpdateDetector(updateKeeper, updateListener),
  m_updateTimer(updateListener),
  m_targetWin(0),
  m_hookInstaller(0),
  m_log(log)
{
#ifndef _WIN64
  m_log->debug("Loading the screenhook library for 32bit system");
#else
  m_log->debug("Loading the screenhook library for 64bit system");
#endif
  try {
    m_hookInstaller = new HookInstaller();
  } catch (::remoting::Exception &e) {
    Thread::terminate();
    m_log->error("Failed to load the hook library: {}", e.getMessage());
  }
  HINSTANCE hinst = GetModuleHandle(0);
  m_targetWin = new MessageWindow(hinst,
  HookDefinitions::HOOK_TARGET_WIN_CLASS_NAME);
}

HooksUpdateDetector::~HooksUpdateDetector()
{
  terminate();
  wait();

  if (m_hookInstaller != 0) {
    delete m_hookInstaller;
  }
  if (m_targetWin != 0) {
    delete m_targetWin;
  }
}

void HooksUpdateDetector::onTerminate()
{
  if (m_targetWin != 0) {
    PostMessage(m_targetWin->getHWND(), WM_QUIT, 0, 0);
  }
  m_initWaiter.notify();
}

void HooksUpdateDetector::start32Loader()
{
#ifdef _WIN64
  m_log->debug("Loading the screenhook library for 32bit system with hookldr.exe");
  if (!isTerminating()) {
    ::string path, folder;
    Environment::getCurrentModuleFolderPath(&folder);
    path.formatf("{}\\{}", folder,
                HookDefinitions::HOOK_LOADER_NAME);
    m_hookLoader32.setFilename(path);
    ::string hwndStr;
    hwndStr.formatf("%I64u", (DWORD64)m_targetWin->getHWND());
    m_hookLoader32.setArguments(hwndStr);
    try {
      m_hookLoader32.start();
    } catch (::remoting::Exception &e) {
      m_log->error("Can't run the 32-bit hook loader: {}", e.getMessage());
    }
  }
#endif
}

void HooksUpdateDetector::terminate32Loader()
{
  if (m_hookLoader32.getProcessHandle() != 0) {
    // Send broadcast message to close the 32 bit hook loader.
    broadcastMessage(HookDefinitions::LOADER_CLOSE_CODE);
  }
}

void HooksUpdateDetector::broadcastMessage(UINT message)
{
  HWND hwndFound = FindWindowEx(HWND_MESSAGE, 0, 0, 0);
  while(hwndFound) {
    PostMessage(hwndFound, message, 0, 0);
    hwndFound = GetNextWindow(hwndFound, GW_HWNDNEXT);
  }
}

void HooksUpdateDetector::execute()
{
  m_log->information("Hooks update detector thread id = {}", getThreadId());

  if (!isTerminating() && m_targetWin != 0) {
    m_targetWin->createWindow();
    m_log->information("Hooks target window has been created (hwnd = {})",
              m_targetWin->getHWND());
  }

  try {
    UipiControl uipiControl(m_log);
    uipiControl.allowMessage(HookDefinitions::SPEC_IPC_CODE,
                             m_targetWin->getHWND());
  } catch (::remoting::Exception &e) {
    terminate();
    m_log->error(e.getMessage());
  }

  bool hookInstalled = false;
  while (!isTerminating() && !hookInstalled) {
    try {
      m_hookInstaller->install(m_targetWin->getHWND());
      hookInstalled = true;
    } catch (::remoting::Exception &e) {
      m_log->error("Hooks installing failed, wait for the next trying: {}",
                 e.getMessage());
      m_initWaiter.waitForEvent(5000);
      try {
        m_hookInstaller->uninstall();
      } catch (::remoting::Exception &e) {
        m_log->error("Hooks uninstalling failed: {}",
                   e.getMessage());
      }
    }
  }

  start32Loader();

  if (!isTerminating()) {
    m_log->information("Hooks update detector has been successfully initialized");
  }

  MSG msg;
  while (!isTerminating()) {
    if (PeekMessage(&msg, m_targetWin->getHWND(), 0, 0, PM_REMOVE) != 0) {
      if (msg.message == HookDefinitions::SPEC_IPC_CODE) {
        ::int_rectangle rect((short)(msg.wParam >> 16), (short)(msg.wParam & 0xffff),
                  (short)(msg.lParam >> 16), (short)(msg.lParam & 0xffff));
         if (rect.has_area())
         {
          m_updateKeeper->addChangedRect(rect);
          m_updateTimer.sear();
        }
//        m_log->debug("Screenhook update rectangle: {x={}, y={}, w={}, h={}}", rect.left, rect.top, rect.width(), rect.height());
      } else {
        DispatchMessage(&msg);
      }
    } else {
      if (WaitMessage() == 0) {
        m_log->error("Hooks update detector has failed");
        Thread::terminate();
      }
    }
  }

  try {
    if (m_hookInstaller != 0) {
      m_hookInstaller->uninstall();
    }
    terminate32Loader();
  } catch (::remoting::Exception &e) {
    m_log->error("{}", e.getMessage());
  }
  m_log->information("Hooks update detector has been terminated.");
}
