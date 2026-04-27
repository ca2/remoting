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

#pragma once


#include "subsystem/_common_header.h"
#include "subsystem/thread/GuiThread.h"
//#include "subsystem/thread/lockable_critical_section.h"
//#include "remoting/remoting/win_system/WinTimeMillis.h"
#include "WindowsInputBlocker.h"
#include "acme/parallelization/happening.h"
#include "../InputBlocker.h"
//#include "log_writer/LogWriter.h"


namespace remoting
{
   // Only one instance of this class  may be created.

   struct _windows_input_blocker_t;


   class CLASS_DECL_REMOTING WindowsInputBlocker :
   virtual public InputBlocker,
    virtual public ::subsystem::GuiThread
   {
   public:


      WindowsInputBlocker();
      ~WindowsInputBlocker() override;


      void initialize_input_blocker(subsystem::LogWriter *plogwriter) override;

      // This functions set/unset blocks on a local keyboard and mouse.
      virtual void setKeyboardBlocking(bool block);
      virtual void setMouseBlocking(bool block);
      // This functions set/unset blocks on a local keyboard and mouse on the
      // timeInterval interval from a last software event generation.
      virtual void setSoftKeyboardBlocking(bool block, unsigned int timeInterval);
      virtual void setSoftMouseBlocking(bool block, unsigned int timeInterval);

      class ::time getLastInputTime() const override;
      void correctLastTime(const class ::time & time) override;

      virtual bool isRemoteInputAllowed();

   //protected:
      virtual void execute();
      virtual void onTerminate();

      bool setKeyboardFilterHook(bool block);
      bool setSoftKeyboardFilterHook(bool block);
      bool setMouseFilterHook(bool block);
      bool setSoftMouseFilterHook(bool block);

      _windows_input_blocker_t  *    m_pwindowsinputblocker;

      static WindowsInputBlocker *s_pwindowsinputblocker;
      static lockable_critical_section s_criticalsection;


      bool m_isKeyboardBlocking;
      bool m_isMouseBlocking;

      bool m_isSoftKeyboardBlocking;
      bool m_isSoftMouseBlocking;
      unsigned int m_timeInterval;
      class ::time  m_lastInputTime;
      lockable_critical_section m_lastInputTimeMutex;


      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };

   //// __WINDOWSINPUTBLOCKER_H__


} // namespace remoting





