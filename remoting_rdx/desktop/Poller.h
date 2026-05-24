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
// with this program; if not, w_rite to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#pragma once


#include "UpdateDetector.h"
#include "ScreenGrabber.h"
#include "innate_subsystem/framebuffer/Framebuffer.h"

#include "acme/parallelization/happening.h"
//#include "log_writer/LogWriter.h"

#define DEFAULT_SLEEP_TIME 1000


namespace remoting_rfb
{

   class CLASS_DECL_REMOTING_RDX Poller :
   virtual public UpdateDetector
   {
   public:

      ::pointer < ScreenGrabber  > m_pscreengrabber;
      ::innate_subsystem::Framebuffer *m_pframebufferBackup;
      lockable_critical_section *m_pcriticalsectionFramebuffer;
      ::i32_rectangle m_rectanglePolling;
      ::happening m_intervalWaiter;
      ::pointer < ::subsystem::LogWriter > m_plogwriter;
      ::pointer < ::remoting_rfb_node::Configurator > m_pconfigurator;

      // Poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
      //        ::innate_subsystem::Framebuffer *backupFramebuffer, lockable_critical_section *framebufferCriticalSection,
      //        ::subsystem::LogWriter * plogwriter);
      Poller();
      ~Poller() override;
      
      void destroy() override;


      virtual void initialize_poller(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ScreenGrabber *pscreengrabber,
       ::innate_subsystem::Framebuffer *backupFramebuffer, lockable_critical_section *framebufferCriticalSection,
       ::subsystem::LogWriter * plogwriter);

      void onThreadMain() override;
      void onTermThread() override;

   };


} // namespace remoting_rfb







