// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once

#include "remoting/remoting_rfb_windows/_common_header.h"
#include "subsystem/thread/GuiThread.h"
//#include "thread/LocalMutex.h"
//#include "win-system/WinTimeMillis.h"
//#include "win-system/WindowsEvent.h"
#include "remoting/remoting_rfb/desktop/InputBlocker.h"
//#include "log-writer/LogWriter.h"

// Only one instance of this class may be created.

namespace remoting_rfb_windows
{
   class CLASS_DECL_REMOTING_RFB_WINDOWS WindowsInputBlocker :
      virtual public ::remoting_rfb::InputBlocker,
   virtual public ::subsystem::GuiThread
   {
   public:

      static HHOOK m_hKeyboardHook;
      static HHOOK m_hSoftKeyboardHook;
      static HHOOK m_hMouseHook;
      static HHOOK m_hSoftMouseHook;

      static WindowsInputBlocker *m_instance;
      static lockable_critical_section m_instanceMutex;

      bool m_isKeyboardBlocking;
      bool m_isMouseBlocking;

      bool m_isSoftKeyboardBlocking;
      bool m_isSoftMouseBlocking;

      static ::u32 m_timeInterval;
      static class ::time m_lastInputTime;
      static lockable_critical_section m_lastInputTimeMutex;

      ::subsystem::LogWriter *m_plogwriter;
      //WindowsInputBlocker(::subsystem::LogWriter *log);
      WindowsInputBlocker();
      ~WindowsInputBlocker() override;


      virtual void initialize_windows_input_blocker(::subsystem::LogWriter *log);

      // This functions set/unset blocks on a local keyboard and mouse.
      void setKeyboardBlocking(bool block) override;
      void setMouseBlocking(bool block) override;
      // This functions set/unset blocks on a local keyboard and mouse on the
      // timeInterval interval from a last software event generation.
      void setSoftKeyboardBlocking(bool block, ::u32 timeInterval) override;
      void setSoftMouseBlocking(bool block, ::u32 timeInterval) override;

      class ::time getLastInputTime() const override;
      void correctLastTime(const class ::time & time) override;

      bool isRemoteInputAllowed() override;

   //protected:
      void onThreadMain() override;
      void onTermThread() override;

      bool setKeyboardFilterHook(bool block);
      bool setSoftKeyboardFilterHook(bool block);
      bool setMouseFilterHook(bool block);
      bool setSoftMouseFilterHook(bool block);

      static LRESULT CALLBACK lowLevelKeyboardFilterProc(::i32 nCode,
                                                         WPARAM wParam,
                                                         LPARAM lParam);
      static LRESULT CALLBACK lowLevelSoftKeyboardFilterProc(::i32 nCode,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
      static  LRESULT CALLBACK lowLevelMouseFilterProc(::i32 nCode,
                                                      WPARAM wParam,
                                                      LPARAM lParam);
      static LRESULT CALLBACK lowLevelSoftMouseFilterProc(::i32 nCode,
                                                          WPARAM wParam,
                                                          LPARAM lParam);


   };
} // namespace remoting_rfb_windows



