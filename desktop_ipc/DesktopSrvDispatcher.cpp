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
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//
#include "framework.h"
#include "DesktopSrvDispatcher.h"
#include "ReconnectException.h"
#include "remoting/util/CommonHeader.h"

DesktopSrvDispatcher::DesktopSrvDispatcher(BlockingGate *gate,
                                           AnEventListener *extErrorListener,
                                           LogWriter *log)
: m_gate(gate),
  m_extErrorListener(extErrorListener),
  m_log(log)
{
}

DesktopSrvDispatcher::~DesktopSrvDispatcher()
{
  terminate();
  wait();
}

void DesktopSrvDispatcher::onTerminate()
{
}

void DesktopSrvDispatcher::notifyOnError()
{
  if (m_extErrorListener) {
    m_extErrorListener->onAnObjectEvent();
  }
}

void DesktopSrvDispatcher::execute()
{
  while (!isTerminating()) {
    try {
      m_log->debug("DesktopSrvDispatcher reading code");
      unsigned char code = m_gate->readUInt8();
      m_log->debug("DesktopSrvDispatcher, code {} recieved", code);
      ::map<unsigned char, ClientListener *>::iterator iter = m_handlers.find(code);
      if (iter == m_handlers.end()) {
        ::string errMess;
        errMess.formatf("Unhandled {} code has been "
                       "received from a client",
                       (int)code);
        throw ::remoting::Exception(errMess);
      }
      (*iter).second->onRequest(code, m_gate);
    } catch (ReconnectException &) {
      m_log->debug("The DesktopServerApplication dispatcher has been reconnected");
    } catch (::exception &e) {
      m_log->error("The DesktopServerApplication dispatcher has "
                 "failed with error: {}", e.get_message());
      notifyOnError();
      terminate();
    }
    Thread::yield();
  }
  m_log->debug("The DesktopServerApplication dispatcher has been stopped");
}

void DesktopSrvDispatcher::registerNewHandle(unsigned char code, ClientListener *listener)
{
  m_handlers[code] = listener;
}
