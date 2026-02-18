// Copyright (C) 2013 GlavSoft LLC.
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

#pragma once


//#include "util/::earth::time.h"
#include "region/Region.h"
//#include "util/::string_array.h"
#include "ScreenDriver.h"
#include "thread/LocalMutex.h"
//#include "log_writer/LogWriter.h"
#include "thread/Thread.h"
#include "win_system/WindowsEvent.h"

class WinVideoRegionUpdaterImpl : public ScreenDriver, Thread
{
public:
  WinVideoRegionUpdaterImpl(LogWriter *log);
  virtual ~WinVideoRegionUpdaterImpl();
protected:
  virtual void execute();
  virtual void onTerminate();
private:
  virtual Region getVideoRegion();
  void updateVideoRegion();
  void getClassNamesAndRectsFromConfig(::string_array &classNames, ::array_base<::int_rectangle> &rects);
  unsigned int WinVideoRegionUpdaterImpl::getInterval();
  Region getRectsByClass(::string_array classNames);
  Region getRectsByCoords(::array_base<::int_rectangle> &rects);

  ::earth::time m_lastVidUpdTime;
  Region m_vidRegion;
  LocalMutex m_regionMutex;
  LogWriter *m_log;
  WindowsEvent m_sleeper;
};

//// __WINVIDEOREGIONUPDATERIMPL_H__
