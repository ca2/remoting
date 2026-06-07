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
#include "framework.h"
#include "RfbDispatcher.h"
#include "subsystem/node/SystemException.h"

namespace remoting_rfb
{



   RfbDispatcher::RfbDispatcher(::remoting_rfb::RfbInputGate *prfbinputgate,
                                const ::procedure &procedureTermination):
Thread("rfbDspt"),
    m_prfbinputgate(prfbinputgate),
     m_procedureTermination(procedureTermination)
     //m_terminationEvent(0)
   {
   }

   // RfbDispatcher::RfbDispatcher(::remoting_rfb::RfbInputGate *pblockinggate,
   //                              ::happening *terminationEvent)
   // : m_pcontrolgate(pcontrolgate),
   //   m_extTerminationListener(0),
   //   m_terminationEvent(terminationEvent)
   // {
   // }

   RfbDispatcher::~RfbDispatcher()
   {
      //setThreadToFinish();
      //waitThreadToFinish();
   }

void RfbDispatcher::destroy()
{
   
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
}


   void RfbDispatcher::notifyAbTermination()
   {
      //if (m_extTerminationListener) {
        // m_extTerminationListener->onAnObjectEvent();
      //}
      if (m_procedureTermination)
      {
         m_procedureTermination();

      }
      //if (m_terminationEvent) {
//         m_terminationEvent->notify();
  //    }
   }

   void RfbDispatcher::onThreadMain()
   {
      try {
         while (!isThreadTerminating()) {
            ::u32 code = m_prfbinputgate->readUInt8();
            if (code == 0xfc) { // special Remoting code
               code = code << 24;
               code += m_prfbinputgate->readUInt8() << 16;
               code += m_prfbinputgate->readUInt8() << 8;
               code += m_prfbinputgate->readUInt8();
            }
            ::map<::u32, RfbDispatcherListener *>::iterator iter = m_handlers.find(code);
            if (iter == m_handlers.end()) {
               ::string errMess;
               errMess.format("unhandled {} code has been received from a client",
                              (::i32)code);
               throw ::subsystem::Exception(errMess);
            }
            (*iter).m_element2->onRequest(code, m_prfbinputgate);
         }
      } catch (...) {
      }
      notifyAbTermination();
   }

   void RfbDispatcher::registerNewHandle(::u32 code, RfbDispatcherListener *listener)
   {
      m_handlers[code] = listener;
   }
} // namespace remoting_rfb


