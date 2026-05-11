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


#include "UpdateDetector.h"
#include "acme/parallelization/happening.h"
#include "HookInstaller.h"
//#include "subsystem_windows/node/MessageWindow.h"
#include "HookUpdateTimer.h"
#include "subsystem/node/Process.h"
//#include "log_writer/LogWriter.h"


namespace remoting
{

   class CLASS_DECL_REMOTING HooksUpdateDetector :
   virtual public UpdateDetector
   {
   public:
      //HooksUpdateDetector(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ::subsystem::LogWriter * plogwriter);
      HooksUpdateDetector();
      ~HooksUpdateDetector() override;


      virtual void initialize_hooks_update_detector(UpdateKeeper * pupdatekeeper, UpdateListener * pupdatelistener, ::subsystem::LogWriter * plogwriter);

   //protected:
      virtual void execute();
      virtual void onTerminate();

      void start32Loader();
      void terminate32Loader();

      ::happening m_initWaiter;

      HookInstaller *m_hookInstaller;
#ifdef WINDOWS
      ::pointer < ::subsystem_windows::MessageWindow  > m_pmessagewindowTarget;
#endif
      ::pointer < HookUpdateTimer > m_phookupdatetimer;
      ::subsystem::Process m_hookLoader32;

   //private:
      void broadcastMessage(unsigned int scopedstrMessage);

      static ::string_literal HOOK_LOADER_NAME;

      ::pointer < ::subsystem::LogWriter > m_plogwriter;
   };


} // namespace remoting
