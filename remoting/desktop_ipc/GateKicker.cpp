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
#include "framework.h"
#include "GateKicker.h"
//#include "subsystem/thread/lockable_critical_section.h"

namespace remoting
{


   // GateKicker::GateKicker(BlockingGate *pblockinggate) : m_pcontrolgate(nullptr)
   // {
   //    //resume();
   // }
   GateKicker::GateKicker() : m_pcontrolgate(nullptr)
   {
      //resume();
   }

   GateKicker::~GateKicker()
   {
      //setThreadToFinish();
      //waitThreadToFinish();
   }

void GateKicker::destroy()
{
   ::subsystem::Thread::destroy();
   // terminateThread();
   // waitThreadToFinish();
}


void GateKicker::initialize_gate_kicker(BlockingGate *pblockinggate)
   {
      m_pcontrolgate = pblockinggate;
      resumeThread();
   }
   void GateKicker::onTermThread() { m_happeningSleeper.set_happening(); }

   void GateKicker::onThreadMain()
   {
      while (!isThreadTerminating())
      {
         m_happeningSleeper.wait(500 * 1_ms);
         if (!isThreadTerminating())
         {
            try
            {
               critical_section_lock al(m_pcontrolgate);
               m_pcontrolgate->writeUInt8(255);
            }
            catch (...)
            {
            }
         }
      }
   }


} // namespace remoting


