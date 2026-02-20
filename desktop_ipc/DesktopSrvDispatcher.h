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


#include "remoting/remoting_common/thread/GuiThread.h"
#include "BlockingGate.h"
#include "ClientListener.h"
#include "remoting/remoting_common/util/AnEventListener.h"
#include "remoting/remoting_common/win_system/WindowsEvent.h"
//#include "log_writer/LogWriter.h"
//#include <map>

class DesktopSrvDispatcher: public GuiThread
{
public:
  DesktopSrvDispatcher(BlockingGate *gate,
                       AnEventListener *m_extTerminationListener,
                       LogWriter *log);
  virtual ~DesktopSrvDispatcher();

  void registerNewHandle(unsigned char code, ClientListener *listener);

protected:
  virtual void execute();
  virtual void onTerminate();
  void notifyOnError();

  BlockingGate *m_gate;

  ::map<unsigned char, ClientListener *> m_handlers;

  AnEventListener *m_extErrorListener;

  LogWriter *m_log;
};

//// __DESKTOPSRVDISPATCHER_H__
