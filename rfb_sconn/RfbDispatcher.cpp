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
#include "RfbDispatcher.h"

RfbDispatcher::RfbDispatcher(RfbInputGate *gate,
                             AnEventListener *extTerminationListener)
: m_gate(gate),
  m_extTerminationListener(extTerminationListener),
  m_terminationEvent(0)
{
}

RfbDispatcher::RfbDispatcher(RfbInputGate *gate,
                             WindowsEvent *terminationEvent)
: m_gate(gate),
  m_extTerminationListener(0),
  m_terminationEvent(terminationEvent)
{
}

RfbDispatcher::~RfbDispatcher()
{
  terminate();
  wait();
}

void RfbDispatcher::notifyAbTermination()
{
  if (m_extTerminationListener) {
    m_extTerminationListener->onAnObjectEvent();
  }
  if (m_terminationEvent) {
    m_terminationEvent->notify();
  }
}

void RfbDispatcher::execute()
{
  try {
    while (!isTerminating()) {
      unsigned int code = m_gate->readUInt8();
      if (code == 0xfc) { // special TightVNC code
        code = code << 24;
        code += m_gate->readUInt8() << 16;
        code += m_gate->readUInt8() << 8;
        code += m_gate->readUInt8();
      }
      ::map<unsigned int, RfbDispatcherListener *>::iterator iter = m_handlers.find(code);
      if (iter == m_handlers.end()) {
        ::string errMess;
        errMess.formatf("unhandled {} code has been received from a client",
                       (int)code);
        throw ::remoting::Exception(errMess);
      }
      (*iter).second->onRequest(code, m_gate);
    }
  } catch (...) {
  }
  notifyAbTermination();
}

void RfbDispatcher::registerNewHandle(unsigned int code, RfbDispatcherListener *listener)
{
  m_handlers[code] = listener;
}
