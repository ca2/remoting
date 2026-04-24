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
#include "RfbDispatcherListener.h"
//#include "subsystem/platform/AnEventListener.h"
//#include "remoting/remoting/win_system/WindowsEvent.h"
//#include aaa_<map>

namespace remoting
{


   class RfbDispatcher : public ::subsystem::Thread
   {
   public:
      // RfbDispatcher(RfbInputGate *gate,
      //               AnEventListener *m_extTerminationListener);
      // RfbDispatcher(RfbInputGate *gate,
      //               WindowsEvent *terminationEvent);
      RfbDispatcher(::remoting::RfbInputGate *gate, const ::procedure &procedureTermination);
      virtual ~RfbDispatcher();

      void registerNewHandle(unsigned int code, RfbDispatcherListener *listener);

   protected:
      virtual void execute();
      void notifyAbTermination();

      ::remoting::RfbInputGate *m_gate;

      ::map<unsigned int, RfbDispatcherListener *> m_handlers;

      ::procedure m_procedureTermination;
      // AnEventListener *m_extTerminationListener;
      // WindowsEvent *m_terminationEvent;
   };



} // namespace remoting


