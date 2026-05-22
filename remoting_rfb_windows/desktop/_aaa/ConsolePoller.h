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

#pragma once


#include "UpdateDetector.h"
#include "ScreenGrabber.h"
#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"

namespace remoting_rfb_windows
{

   class CLASS_DECL_REMOTING_RFB ConsolePoller :
   virtual public UpdateDetector
   {
   public:
      ConsolePoller();

      virtual ~ConsolePoller();
      virtual void initialize_console_poller(::remoting_rfb::UpdateKeeper * pupdatekeeper, ::remoting_rfb::UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
                    ::innate_subsystem::Framebuffer *backupFramebuffer, lockable_critical_section *framebufferMutex,
                    ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void onThreadMain();
      virtual void onTermThread();

   //private:
      ::i32_rectangle getConsoleRect();

      ::pointer < ScreenGrabber > m_pscreengrabber;
      ::i32_rectangle m_rectanglePolling;
      ::happening m_intervalWaiter;
      ::pointer < ::innate_subsystem::Framebuffer  > m_pframebufferBackup;
      lockable_critical_section *m_pcriticalsectionFramebuffer;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_rfb_windows






