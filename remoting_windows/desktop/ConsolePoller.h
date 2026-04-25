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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "UpdateDetector.h"
#include "ScreenGrabber.h"
#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"

namespace remoting
{

   class CLASS_DECL_REMOTING ConsolePoller :
   virtual public UpdateDetector
   {
   public:
      ConsolePoller();

      virtual ~ConsolePoller();
      virtual void initialize_console_poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
                    ::innate_subsystem::Framebuffer *backupFramebuffer, critical_section *framebufferMutex,
                    ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void execute();
      virtual void onTerminate();

   //private:
      ::int_rectangle getConsoleRect();

      ::pointer < ScreenGrabber > m_pscreengrabber;
      ::int_rectangle m_rectanglePolling;
      ::happening m_intervalWaiter;
      ::pointer < ::innate_subsystem::Framebuffer  > m_pframebufferBackup;
      critical_section *m_pcriticalsectionFramebuffer;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting






