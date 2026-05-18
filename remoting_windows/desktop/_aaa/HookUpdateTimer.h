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


#include "subsystem/thread/Thread.h"
#include "::remoting::UpdateListener.h"
#include "acme/parallelization/happening.h"

namespace remoting_windows
{

   // This class  is a timer that after calling the sear() function
   // wait a time interval after that it notifies to an update listener
   // for update/updates catching. It's should to help the HooksupdateDetector
   // to wait an time interval because the HooksupdateDetector can't wait
   // directly by sleep. Usage of this class  is questionable.
   class CLASS_DECL_REMOTING HookUpdateTimer :
   virtual public ::subsystem::Thread
   {
   public:

      // @param pupdatelistener - pointer to an ::remoting::UpdateListener object
      HookUpdateTimer();
      ~HookUpdateTimer() override;


      virtual void initialize_hook_update_timer(::remoting::UpdateListener *pupdatelistener);

      // This function start a timer after that will calling onUpdate()
      // function of the update listener
      void sear();

   //protected:
      virtual void execute();
      virtual void onTerminate();

      ::happening m_updateWaiter;
      ::happening m_happeningTimer;
      ::pointer < ::remoting::UpdateListener  > m_pupdatelistener;
   };


} // namespace remoting_windows
