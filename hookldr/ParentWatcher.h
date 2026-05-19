// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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


#include "remoting/remoting/win_system/ParentProcess.h"
#include "ParentEventsListener.h"
#include "acme/parallelization/happening.h"
#include "subsystem/_common_header.h"
#include "subsystem/thread/Thread.h"
// #include aaa_<Tlhelp32.h>

class CLASS_DECL_REMOTING ParentWatcher : private Thread
{
public:
  ParentWatcher(ParentEventsListener *parentEventListener);
  virtual ~ParentWatcher();

private:
  virtual void onThreadMain();
  virtual void onTermThread();

  ParentEventsListener *m_parentEventListener;
  ParentProcess m_parentProcess;
  ::happening m_happeningTimer;
};

//// __PARENTWATCHER_H__
