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
#include "acme/parallelization/happening.h"
#include "HookInstaller.h"
//#include "subsystem_windows/node/MessageWindow.h"
#include "HookUpdateTimer.h"
#include "subsystem/node/Process.h"
//#include "log_writer/LogWriter.h"


namespace remoting_windows
{

   class CLASS_DECL_REMOTING HooksUpdateDetector :
   virtual public UpdateDetector
   {
   public:
      //HooksUpdateDetector(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener, ::subsystem::LogWriter * plogwriter);
      HooksUpdateDetector();
      ~HooksUpdateDetector() override;


      virtual void initialize_hooks_update_detector(::remoting_rtc::UpdateKeeper * pupdatekeeper, ::remoting_rtc::UpdateListener * pupdatelistener, ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void onThreadMain();
      virtual void onTermThread();

      void start32Loader();
      void terminate32Loader();

      ::happening m_initWaiter;

      HookInstaller *m_hookInstaller;
      ::pointer < ::subsystem_windows::MessageWindow  > m_pmessagewindowTarget;
      ::pointer < HookUpdateTimer > m_phookupdatetimer;
      ::subsystem::Process m_hookLoader32;

   //private:
      void broadcastMessage(unsigned int scopedstrMessage);

      static ::string_literal HOOK_LOADER_NAME;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting_windows
